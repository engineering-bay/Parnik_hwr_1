#include <parnik.h>
#include <delay.h>
#include <string.h>
#include <alcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <gsm.h>

#define MENU_ITEM_TIME          1
#define MENU_ITEM_PHONENUM      2
#define MENU_ITEM_EXIT          3
#define MENU_MAXITEM            3

void MenuLoop(void)
{
    int menuexit = 0, menuitem, key;
    char str1[16], str2[16];
    
    menuitem = MENU_ITEM_TIME;
    while(menuexit == 0)
    {
        switch(menuitem)
        {
            case MENU_ITEM_TIME:
            {
                sprintf(str1, "== SETTINGS ==");
                sprintf(str2, "1) SET TIME");
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_puts(str1);
                lcd_gotoxy(0, 1);
                lcd_puts(str2);
            }break;
            case MENU_ITEM_PHONENUM:
            {
                sprintf(str1, "== SETTINGS ==");
                sprintf(str2, "2) SET PHONE NUM");
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_puts(str1);
                lcd_gotoxy(0, 1);
                lcd_puts(str2);
            }break;
            case MENU_ITEM_EXIT:
            {
                sprintf(str1, "== SETTINGS ==");
                sprintf(str2, "3) EXIT MENU");
                lcd_clear();
                lcd_gotoxy(0, 0);
                lcd_puts(str1);
                lcd_gotoxy(0, 1);
                lcd_puts(str2);
            }break;
        }
        key = -1;
        while(key == -1)
        {
            key = KeyPressed();
            delay_ms(200);
        }
        if(key == 10 && menuitem == MENU_ITEM_EXIT) menuexit = 1; 
        
        if(key == 2) menuitem--;
        if(key == 8) menuitem++;
        if(menuitem > MENU_MAXITEM) menuitem = 1;
        if(menuitem < 1) menuitem = MENU_MAXITEM;
    }
}