/**
 * @defgroup PBA_MENU
 * @{
 *******************************************************************************
 * @file            PBA_menu.c
 * @brief           Menu-Library für das PBA6
 * @author          ICT Berufsbildungscenter AG
 *******************************************************************************
 *
 * @copyright
 * @{
 *
 * Diese Software kann unter den Bedingungen der MIT-Lizenz verwendet werden.
 *
 * Copyright &copy; 2016 ICT Berufsbildungscenter AG
 *
 * #####MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @}
 */
#include "../inc/PBA_config.h"
#ifdef USE_MENU_LIBRARY
/*----- LCD-Konfiguration-------------------------------------------------------------------*/

/*Anzahl Zeilen auf dem LCD*/
#define MAXLINES (2u)       /**< Anzahl Zeilen des Displays */

/*Anzahl Zeichen pro Zeile*/
#define MAXCHARS (16u)      /**< Anzahl Zeichen pro Zeile */

/*Defines Tasterrflanken*/
#define POSEDGE_UP      p_events->posEdge.switch2   /**< Tasterflanke Taste UP */
#define POSEDGE_DOWN    p_events->posEdge.switch1   /**< Tasterflanke Taste Down */
#define POSEDGE_ENTER   p_events->posEdge.switch0   /**< Tasterflanke Taste Enter */


/**
 * @brief Zeichen welches angezeigt wird falls Einträge oberhalb des aktuellen Eintrags vorhanden sind
 * welche nicht angezeigt werden
 */
uint8_t charsetAddrArrowUp      =   0x00;

/**
 * @brief Zeichen welches angezeigt wird falls Einträge unterhalb des aktuellen Eintrags vorhanden sind
 * welche nicht angezeigt werden
 */
uint8_t charsetAddrArrowDown    =   0x01;



/*----- LCD-Funktionen ---------------------------------------------------------------------*/

/**
 * @brief Ruft die Funktion zum Löschen des LCDs auf
 */
static inline void MENU_LcdClear()
{
    LCD_Clear();
}
/**
 * @brief Sendet Daten ans LCD, nötig wenn mit Framebuffer gearbeitet wird
 */
//static inline void MENU_LcdUpdate();
#define MENU_LcdUpdate()


/**
 * @brief Ruft Funktion zum anzeigen eines Strings auf einer bestimmten Zeile auf*
 * @param row       Displayzeile auf welche geschrieben wird
 * @param p_text    Auszugebender Text
 */
static inline void MENU_LcdDisplayStringLine(uint8_t row,const uint8_t *p_text)
{
    LCD_GotoXY(1,row);      /*erste Zeile=0*/
    LCD_PutsRom(p_text);
}

/**
 * @brief Ruft Funktion zum Anzeigen eines Zeichens an bestimmter Stelle auf bestimmter Zeile auf
 * @param row   Displayzeile auf welche geschrieben wird
 * @param posX  Position in Zeile
 * @param ch    Zeichen
 */
static inline void MENU_LcdDisplayCharLine(uint8_t row, uint8_t posX,uint8_t ch)
{
    LCD_GotoXY(posX,row);       /*Erste Zeile=0, Erstes Zeichen=0*/
    LCD_Putc(ch);
}






/*----- PBA spezifische LCD-Funktionen ----------------------------------------------------*/
/**
 * @brief Fügt ein selbst definiertes Zeichen dem Charset eines DOGM-162 Displays hinzu
 * @param cgramAddr CGRAM Adresse an welchem das Zeichen gespreichert werden soll (0..7)
 * @param p_char    Zeichen-Array (8-Byte)
 */
static void LCD_SetChar(unsigned char cgramAddr, unsigned const char *p_char)
{
    unsigned char i;
    LCD_SendByte(0x40|(cgramAddr<<3));          /*CGRAM-Addresse setzen (Befehl)*/
    LCD_RS=1;                                   /*LCD umschalten auf Datenempfang*/
    for(i=0;i<=7;i++)                           /*8 Bytes CG-Daten senden*/
        LCD_SendByte(*(p_char+i));              /*Sende 1 Byte Daten aus übergabe-Array*/
    LCD_RS=0;                                   /*LCD umschalten auf Befehlsempfang*/
}

/**
 * @brief Speichert Pfeil und Ok Zeichen im CGRAM des DOGM162-Displays
 */
static void LCD_CreateArrows(void)
{
    const uint8_t arrow_up[]=  {0b0000100,
                                0b0001110,
                                0b0010101,
                                0b0000100,
                                0b0000100,
                                0b0000100,
                                0b0000100,
                                0b0000000};

    const uint8_t arrow_down[]={0b0000100,
                                0b0000100,
                                0b0000100,
                                0b0000100,
                                0b0010101,
                                0b0001110,
                                0b0000100,
                                0b0000000};

    const uint8_t ok[]=        {0b0000001,
                                0b0000001,
                                0b0000010,
                                0b0000010,
                                0b0000010,
                                0b0010100,
                                0b0001100,
                                0b0000000};
    LCD_SetChar(0,arrow_up);                    /*arrow_up in CGRAM (resp. CGROM) laden*/
    LCD_SetChar(1,arrow_down);                  /*arrow_up in CGRAM (resp. CGROM) laden*/
    LCD_SetChar(2,ok);                          /*ok in CGRAM (resp. CGROM) laden*/
}
/*----- Pointer auf ersten Menueintrag -----------------------------------------------------*/
static menuEntry_t *p_menu; /**< Pointer auf ersten Menueintrag*/

/*----- Pointer auf Events-Struktur --------------------------------------------------------*/

static events_t *p_events; /**< Pointer auf Event-Struktur*/

/*----- Datentyp welcher die Scrollrichtung speichert --------------------------------------*/

/**
 *@brief Aktuelle Scrollrichtung.
 */
typedef enum
{
    DIR_UP,     /**< Aufwärts */
    DIR_DOWN    /**< Abwärts */
}menuScrollDirection_t;

/*----- Initialisierung des Menüs ----------------------------------------------------------*/

/**
 * @brief Initialisierung des Menus
 * @param p_menuStart   Erster Menueintrag der auf dem Display dargestellt werden soll
 * @param p_event       Im Hauptprogramm verwendete Eventstruktur
 */
void MENU_Init(menuEntry_t *p_menuStart ,events_t *p_event)
{
    p_menu=p_menuStart;
    p_events=p_event;
    LCD_CreateArrows();
}


/*----- Zeichnen des Menüs auf das Display -------------------------------------------------*/

/**
 * @brief Zeichet das Menu auf das Display.
 * @param p_firstRowMenuEntry   Menueintrag der zuoberst auf dem Display angezeigt wird
 * @param p_cursorRowMenuEntry  Menueintrag der aktuell angewählt ist
 * @param direction             Scrollrichtung
 */
static void MENU_Draw(menuEntry_t *p_firstRowMenuEntry, menuEntry_t *p_cursorRowMenuEntry,menuScrollDirection_t direction)
{   uint8_t drawRowNmbr=0;      /*1. Zeile auf die geschrieben wird*/
    menuEntry_t *p_currentRowMenuEntry;
    MENU_LcdClear();
    if(((direction==DIR_DOWN)&&((p_cursorRowMenuEntry->id)>(MAXLINES-1)))|| /* nach unten scrollen in menu, aktuelle gewählte ID grösser als maximal anzeigbare Zeilen in Display*/
        ((direction==DIR_UP) && (p_cursorRowMenuEntry->id>(MAXLINES-1)) && (p_cursorRowMenuEntry->next==p_firstRowMenuEntry))) /* nach oben scrollen, aktueller Eintrag = letzter Eintrag*/
    {
        p_currentRowMenuEntry=p_cursorRowMenuEntry;
        drawRowNmbr=(MAXLINES);
        do
        {
            MENU_LcdDisplayStringLine(drawRowNmbr-1,p_currentRowMenuEntry->name);   /*Name des Menueintrags aufs Display zeichen*/
            if(p_currentRowMenuEntry==p_cursorRowMenuEntry)                         /*Wenn aktuelle Zeile=angewählte Zeile*/
            MENU_LcdDisplayCharLine(drawRowNmbr-1,0,'*' );                          /*Markierung aufs Display schreiben*/
            p_currentRowMenuEntry=p_currentRowMenuEntry->prev;                      /*Danach vorherige Zeile anwählen*/
            drawRowNmbr-=1;                                                         /*Zeilennummer verkleinern*/
        }
        while((p_currentRowMenuEntry!=p_firstRowMenuEntry)&&(drawRowNmbr>0));       /*Dies wird wiederholt solange sich die Einträge nicht wiederholen*/

        if(p_cursorRowMenuEntry->next!=p_firstRowMenuEntry)                         /*Weitere Menueinträge unterhalb*/
        {
            MENU_LcdDisplayCharLine((MAXLINES-1),(MAXCHARS-1),charsetAddrArrowDown );/*Pfeil nach unten anzeigen*/
        }
        if(p_cursorRowMenuEntry->id-(MAXLINES-1)>0)                                 /*Weitere Einträge oberhalb*/
        {
            MENU_LcdDisplayCharLine((0),(MAXCHARS-1),charsetAddrArrowUp );                    /*Pfeil nach oben anzeigen*/
        }
    }

    else
    {
        /*nach oben scrollen, aktuell gewählte ID grösser als maximal anzeigbare Zeilen in Display*/
        if((direction==DIR_UP)&&((p_cursorRowMenuEntry->id)>(MAXLINES-1)))
        {
            p_currentRowMenuEntry=p_cursorRowMenuEntry;
            do
            {
                MENU_LcdDisplayStringLine(drawRowNmbr,p_currentRowMenuEntry->name); /*Name des Menueintrags aufs Display zeichen*/
                if(p_currentRowMenuEntry==p_cursorRowMenuEntry)                     /*Wenn aktuelle Zeile=angewählte Zeile*/
                    MENU_LcdDisplayCharLine(drawRowNmbr,0,'*' );                        /*Markierung aufs Display schreiben*/
                p_currentRowMenuEntry=p_currentRowMenuEntry->next;                  /*Danach nächste Zeile anwählen*/
                drawRowNmbr+=1;                                                     /*Zeilennummer erhöhen*/
            }
            while((p_currentRowMenuEntry!=p_firstRowMenuEntry)&&(drawRowNmbr<MAXLINES));/*Dies wird wiederholt solange sich die Einträge nicht wiederholen*/

            if(p_currentRowMenuEntry!=p_firstRowMenuEntry)                          /*Weitere Einträge unerhalb*/
            {
                MENU_LcdDisplayCharLine((MAXLINES-1),(MAXCHARS-1),charsetAddrArrowDown );     /*Pfeil nach unten anzeigen*/
            }
            if(p_cursorRowMenuEntry->id-(MAXLINES-1)>0)                             /*Weitere Einträge oberhalb*/
            {
                MENU_LcdDisplayCharLine((0),(MAXCHARS-1),charsetAddrArrowUp );      /*Pfeil nach oben anzeigen*/
            }
        }
        /*Aktuelle ID des Menueintrags kleiner als maximal anzeigbare Zeilen auf Display*/
        else
        {
            p_currentRowMenuEntry=p_firstRowMenuEntry;
            do
            {
                MENU_LcdDisplayStringLine(drawRowNmbr,p_currentRowMenuEntry->name); /*Name des Menueintrags aufs Display zeichen*/
                if(p_currentRowMenuEntry==p_cursorRowMenuEntry)                     /*Wenn aktuelle Zeile=angewählte Zeile*/
                MENU_LcdDisplayCharLine(drawRowNmbr,0,'*' );                        /*Markierung aufs Display schreiben*/
                p_currentRowMenuEntry=p_currentRowMenuEntry->next;                  /*Danach nächste Zeile anwählen*/
                drawRowNmbr+=1;                                                     /*Zeilennummer erhöhen*/
            }
            while((p_currentRowMenuEntry!=p_firstRowMenuEntry)&&(drawRowNmbr<(MAXLINES)));  /*Dies wird wiederholt solange sich die Einträge nicht wiederholen oder Display voll*/
            if(p_currentRowMenuEntry!=p_firstRowMenuEntry)                                  /*Weitere Einträge unterhalb*/
            {
                MENU_LcdDisplayCharLine((MAXLINES-1),(MAXCHARS-1),charsetAddrArrowDown );   /*Pfeil nach unten anzeigen*/
            }
        }
    }
    MENU_LcdUpdate();
}
/*----- Aktualisieren des Cursorsymbols auf dem Display -------------------------*/

/**
 * @brief Manuelles Setzen der Cursors auf eine bestimmte Zeile
 * @param p_firstRowMenuEntry       Erster Menueintrag
 * @param p_cursorRowMenuEntryNew   Menueintrag bei dem Cursor gesetzt werden soll
 * @param p_cursorRowMenuEntryOld   Menueintrag bei dem der Cursor aktuell ist
 */
static void MENU_SetCursor(menuEntry_t *p_firstRowMenuEntry, menuEntry_t *p_cursorRowMenuEntryNew, menuEntry_t *p_cursorRowMenuEntryOld)
{  uint8_t drawRowNmbr=0;
   menuEntry_t *p_currentRowMenuEntry;

   p_currentRowMenuEntry=p_firstRowMenuEntry;
   do
   {
        if(p_currentRowMenuEntry==p_cursorRowMenuEntryNew)  /*Entspricht aktuelle Zeile, der neuen angewählten Zeile*/
        {
            MENU_LcdDisplayCharLine(drawRowNmbr,0,'*' );    /*Markierung schreiben*/
        }
        if(p_currentRowMenuEntry==p_cursorRowMenuEntryOld)  /*Entspricht aktuelle Zeile der alten angewählten Zeile*/
        {
            MENU_LcdDisplayCharLine(drawRowNmbr,0,' ' );    /*Markierung löschen*/
        }
       p_currentRowMenuEntry=p_currentRowMenuEntry->next;   /*Nächste Zeile anwählen*/
       drawRowNmbr+=1;                                      /*Zeilennummer erhöhen*/

   }
   while(p_currentRowMenuEntry!=p_firstRowMenuEntry);
}

/* ----- Hauptfunktion des Menüs -------------------------------------------------*/

/**
 * @brief Anzeige und Aktualisierung des Menus.
 * Muss zyklisch aufgerufen werden
 * @return 1:Menu beendet, 0:Menu nicht beendet
 */
uint8_t MENU_Call (void)
{
    static menuEntry_t *p_cursorRowMenuEntry=0,*p_firstRowMenuEntry=0;

    static uint8_t isFirstCall = 1;
    static uint8_t doRepeatFunction = 0;
    static uint8_t drawAfterReturn = 0;

    if(isFirstCall)     /*Wird die Funktion das erste Mal aufgerufen->Startwerte setzen*/
    {
        p_cursorRowMenuEntry=p_firstRowMenuEntry=p_menu;    /*Startpunkt Hauptmenü*/

        MENU_Draw(p_firstRowMenuEntry,p_cursorRowMenuEntry,DIR_UP);
        isFirstCall=0;
    }
    if(doRepeatFunction==0)         /*Wenn im vorherigen Aufruf keine Funktion ausgwählt wurde die wiederholt werden muss*/
    {                               /*Überprüfung der Tasten und Menühandling*/

        if( POSEDGE_UP )            /* Taste "Rauf" gedrückt ?*/
        {
            p_cursorRowMenuEntry=p_cursorRowMenuEntry->prev;            /*Position speichern*/
            MENU_Draw(p_firstRowMenuEntry,p_cursorRowMenuEntry,DIR_UP);

             return 0;

        }
        if( POSEDGE_DOWN )          /* Taste "Runter" gedrückt?*/
        {
            p_cursorRowMenuEntry=p_cursorRowMenuEntry->next;            /*Position speichern*/
            MENU_Draw(p_firstRowMenuEntry,p_cursorRowMenuEntry,DIR_DOWN);

            return 0;
        }
        if( POSEDGE_ENTER )     /*Taste "Enter" gedrückt?*/
        {
            p_events->valueEdge=0;   /* Verhindern, dass Tastendruck in Unterfunktion noch erkannt wird */
            if(p_cursorRowMenuEntry->submenu)                       /*Wenn ein Untermenü vorhanden*/
            {
                p_firstRowMenuEntry=p_cursorRowMenuEntry=p_cursorRowMenuEntry->submenu; /*Cursor neu setzen*/
                MENU_Draw(p_firstRowMenuEntry, p_cursorRowMenuEntry,DIR_UP);    /*Untermenu zeichnen*/
                return 0;
            }
            else
            {
                if(p_cursorRowMenuEntry->function)      /*Wenn eine Funktion vorhanden*/
                {
                    p_cursorRowMenuEntry->function(1);  /*Funktion ausführen, Übergabeparameter=1->1. Aufruf*/
                    doRepeatFunction=1;                 /*Merkvariable setzen, beim nächsten Aufruf von menu() wird wieder die Funktion ausgeführt*/
                    return 0;
                }
                else
                {
                    if(p_cursorRowMenuEntry->ret)       /*Wenn ein Rückkehrmenu vorhanden*/
                    {
                        p_cursorRowMenuEntry=p_firstRowMenuEntry=p_cursorRowMenuEntry->ret; /*Cursor auf Rückkehrmenu setzen*/
                        MENU_Draw(p_firstRowMenuEntry,p_cursorRowMenuEntry,DIR_UP); /*Menu zeichnen*/
                        return 0;
                    }
                    else                                /*Falls weder Untermenu, noch Funktion, noch Rückkehrmenu vorhanden*/
                    {                                   /*Kann nur bei Exit im Hauptmenu vorkommen*/
                        isFirstCall=1;                  /*Startvariable wieder setzen*/
                        return 1;                       /*Menu beendet*/
                    }
                }
            }
        }
        if( drawAfterReturn )
        {
            p_firstRowMenuEntry=p_cursorRowMenuEntry=p_cursorRowMenuEntry->ret; /*Cursor auf Rückkehrmenu setzen*/
            MENU_Draw(p_firstRowMenuEntry, p_cursorRowMenuEntry,DIR_UP);        /*Menu zeigen*/
            drawAfterReturn = 0;
        }
        return 0;
    }
    else/*Falls repeatfunction gesetzt (Funktion soll noch einmal ausgeführt werden)*/
    {
        if(p_cursorRowMenuEntry->function(0))   /*Falls die ausgeführt Funktion 1 als Rückgabe liefert*/
        {                                       /*Funktion muss nicht wieder ausgeführt werden*/
            doRepeatFunction=0;                 /*Speichervariabel löschen*/
            drawAfterReturn = 1;                /* Menü muss bei der nächsten Ausführung neu gezeichnet werden */
        }
    return 0;
    }
}
#endif

/**
 * @}
 */