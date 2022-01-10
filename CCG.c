#define Key_On 0
#define Key_Off 1 // set the high and low level

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions
#include <time.h>  // time() to get time and generate random

#include "ece198.h"

bool StartGame();
void RanColour();
void ChangeColour();
void SetPins(GPIO_TypeDef *bluePort, uint16_t bluePin, GPIO_TypeDef *greenPort, uint16_t greenPin, GPIO_TypeDef *redPort, uint16_t redPin, int color);
bool Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void CheckAnswer( int answer[], int index);
void SetLives();
void ReadLevels(uint16_t GPIO_Pin1, uint16_t GPIO_Pin2, uint16_t GPIO_Pin3, uint16_t GPIO_Pin4, uint16_t GPIO_Pin5, 
            GPIO_PinState state_1,  GPIO_PinState state_2, GPIO_PinState state_3, GPIO_PinState state_4, GPIO_PinState state_5);
void SetLevels();
void ReadLives(uint16_t GPIO_Pin1, uint16_t GPIO_Pin2, uint16_t GPIO_Pin3, GPIO_PinState state_1,  GPIO_PinState state_2, GPIO_PinState state_3);
bool Scan_MasterButton();


int S = 0;
int R = 0;
int i = 0;
int G = 0;
int V = 0;
int Min = 1;
int Max = 5;
int colour1 = 0;
int colour2 = 0;
int colour3 = 0;
int colour4 = 0;
int colour5 = 0;
// GPIO_PinState state1 = 1; 
// GPIO_PinState state2 = 1;
// GPIO_PinState state3 = 1;

// test array
int answer1[] = {1, 5, 3, 2, 3, 4, 2, 1, 5, 4, 3, 2};  // failed once
// int answer2[] = {4, 2, 5};  // failed all three times

int main(void)
{
    HAL_Init(); // initialize the Hardware Abstraction Layer

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // on-board LED

    SerialSetup(9600);

    InitializePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7
                | GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_0 | GPIO_PIN_11 | GPIO_PIN_12,
                GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // initialize color LED output pins
    InitializePin(GPIOB, GPIO_PIN_5 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_0 | GPIO_PIN_10 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13,
                GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // initialize color LED output pins
    InitializePin(GPIOC, GPIO_PIN_1 | GPIO_PIN_0 | GPIO_PIN_8 | GPIO_PIN_6 | GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // initialize color LED output pins            
    
    //initialize button pins 
    InitializePin(GPIOA, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_PULLUP, 0); 
    InitializePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_MODE_INPUT, GPIO_PULLUP, 0); 
    InitializePin(GPIOC, GPIO_PIN_7 | GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0);

    // press any of the answer buttons, the colour changed.
    while(1) 
    { 
        
        while(StartGame()) 
        {
            ChangeColour();
            while ( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)&&
            HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)&& HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)&&
            HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)&& HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7));   // wait for button press // HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_#)&&

            srand(HAL_GetTick()); // or use srand((unsigned)time(NULL)); // get the seed
            CheckAnswer(answer1, 12);
            
            while ( !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)&&
            !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)&& !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)&&
            !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)&& !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)); // wait for button release // !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_#)&&
            
            SetLevels();
            SetLives();

            HAL_Delay(250);

            i++;
        }
    }
    return 0;
}   
    
bool StartGame()
{
    Scan_MasterButton(); // see if the button is pressed
    if(V == true)  // the button be pressed
    {
        G += 1;

        if(G % 2 == 0)
        {
            SetPins(GPIOA, GPIO_PIN_5, GPIOA, GPIO_PIN_6,GPIOA, GPIO_PIN_7, 0);
            SetPins(GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_1, GPIOC, GPIO_PIN_0, 0);
            SetPins(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_4, 0);
            SetPins(GPIOB, GPIO_PIN_4, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_3, 0);
            SetPins(GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_8, GPIOB, GPIO_PIN_10, 0);
            R = 0;
            S = 0;
            i = 0;
            SetLives();
            SetLevels();
            G = 0; 
            return false;
        }
        if(G % 2 == 1)
        {
            SetLevels();
            SetLives();
            return true;
        }
    }
    if(V == false) // the button isn't pressed
    {
        if(G % 2 == 0)
        {
            return false;
        }
        if(G % 2 == 1)
        {
            return true;
        }
    }
}

void RanColour()
{
    while (colour1 == colour2 || colour1 == colour3 || colour1 == colour4 || colour1 == colour5 || colour2 == colour3 ||
        colour2 == colour4 || colour2 == colour5 || colour3 == colour4 || colour3 == colour5 || colour4 == colour5)
    {
        colour1 = (rand() % (Max - Min + 1)) + Min;  // get random number between [1,5]
        colour2 = (rand() % (Max - Min + 1)) + Min;  // get random number between [1,5]
        colour3 = (rand() % (Max - Min + 1)) + Min;  // get random number between [1,5]
        colour4 = (rand() % (Max - Min + 1)) + Min;  // get random number between [1,5]
        colour5 = (rand() % (Max - Min + 1)) + Min;  // get random number between [1,5]
    }
}

void ChangeColour()
{
    RanColour();
    switch (colour1)
    {
        case 1:   //blue, color = 1
            SetPins(GPIOA, GPIO_PIN_5, GPIOA, GPIO_PIN_6,GPIOA, GPIO_PIN_7, 1);
            break;
        case 2:   //green, color = 2
            SetPins(GPIOA, GPIO_PIN_5, GPIOA, GPIO_PIN_6,GPIOA, GPIO_PIN_7, 2);
            break;
        case 3:   //red, color = 4
            SetPins(GPIOA, GPIO_PIN_5, GPIOA, GPIO_PIN_6,GPIOA, GPIO_PIN_7, 4);
            break;
        case 4:   //purple, color = 5
            SetPins(GPIOA, GPIO_PIN_5, GPIOA, GPIO_PIN_6,GPIOA, GPIO_PIN_7, 5);
            break;
        case 5:  //yellow, color = 6
            SetPins(GPIOA, GPIO_PIN_5, GPIOA, GPIO_PIN_6,GPIOA, GPIO_PIN_7, 6);
            break;
    }
    switch (colour2)
    {
        case 1:   //blue
            SetPins(GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_1, GPIOC, GPIO_PIN_0, 1);
            break;
        case 2:   //green
            SetPins(GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_1, GPIOC, GPIO_PIN_0, 2);
            break;
        case 3:   //red
            SetPins(GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_1, GPIOC, GPIO_PIN_0, 4);
            break;
        case 4:   //purple
            SetPins(GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_1, GPIOC, GPIO_PIN_0, 5);
            break;
        case 5:  //yellow
            SetPins(GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_1, GPIOC, GPIO_PIN_0, 6);
            break;
    }   
    switch (colour3)
    {
        case 1:   //blue
            SetPins(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_4, 1);
            break;
        case 2:   //green
            SetPins(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_4, 2);
            break;
        case 3:   //red
            SetPins(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_4, 4);
            break;
        case 4:   //purple
            SetPins(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_4, 5);
            break;
        case 5:  //yellow
            SetPins(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_4, 6);
            break;
    }
    switch (colour4)
    {
        case 1:   //blue
            SetPins(GPIOB, GPIO_PIN_4, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_3, 1);
            break;
        case 2:   //green
            SetPins(GPIOB, GPIO_PIN_4, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_3, 2);
            break;
        case 3:   //red
            SetPins(GPIOB, GPIO_PIN_4, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_3, 4);
            break;
        case 4:   //purple
            SetPins(GPIOB, GPIO_PIN_4, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_3, 5);
            break;
        case 5:  //yellow
            SetPins(GPIOB, GPIO_PIN_4, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_3, 6);
            break;
    }
    switch (colour5)
    {
        case 1:   //blue
            SetPins(GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_8, GPIOB, GPIO_PIN_10, 1);
            break;
        case 2:   //green
            SetPins(GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_8, GPIOB, GPIO_PIN_10, 2);
            break;
        case 3:   //red
            SetPins(GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_8, GPIOB, GPIO_PIN_10, 4);
            break;
        case 4:   //purple
            SetPins(GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_8, GPIOB, GPIO_PIN_10, 5);
            break;
        case 5:  //yellow
            SetPins(GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_8, GPIOB, GPIO_PIN_10, 6);
            break;
    }
}

void SetPins(GPIO_TypeDef *bluePort, uint16_t bluePin,
       GPIO_TypeDef *greenPort, uint16_t greenPin,
       GPIO_TypeDef *redPort, uint16_t redPin, int color)
{
    HAL_GPIO_WritePin(bluePort, bluePin, (color & 0x01) != 0);  // blue  (hex 1 == 0001 binary) 
    HAL_GPIO_WritePin(greenPort, greenPin, (color & 0x02) != 0);  // green (hex 2 == 0010 binary)
    HAL_GPIO_WritePin(redPort, redPin, (color & 0x04) != 0);  // red   (hex 4 == 0100 binary)
}

bool Scan_MasterButton()
{
    while (1)
    {
        if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == Key_On)  // the key be pressed
        {
            while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4)); 
            while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4)); 
            
            V = true;
            return V;
        }
        else
        {
            V = false;
            return V;
        }
    }
}

// scan the button to see if they are pressed or not
bool Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == Key_On)  // the key be pressed
    {
        while(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)); 
        while(!HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)); // ???
        
        return true;
    }
    else
    {
        return false;
    }
}

// check if the user press the correct button 
void CheckAnswer(/*int answer*/ int answer[], int index)
{
    if(answer[i] == colour1) // colour1 is the right answer
    {
        if(Key_Scan(GPIOC, GPIO_PIN_7)) // press the right button
        {
            S++;
        }
        else
        {
            R++;
            S = 0;
        }
    }
    if(answer[i] == colour2)
    {
        if(Key_Scan(GPIOA, GPIO_PIN_10))
        {
            S++;
        }
        else 
        {
            R++;
            S = 0;
        }
    }
    if(answer[i] == colour3)
    {
        if(Key_Scan(GPIOB, GPIO_PIN_9))
        {
            S++;
        }
        else 
        {
            R++;
            S = 0;
        }
    }
    if(answer[i] == colour4)
    {
        if(Key_Scan(GPIOB, GPIO_PIN_6))
        {
            S++;
        }
        else 
        {
            R++;
            S = 0;
        }
    }
    if(answer[i] == colour5)
    {
        if(Key_Scan(GPIOB, GPIO_PIN_8))
        {
            S++;
        }
        else 
        {
            R++;
            S = 0;
        }
    }
    colour1 = 0;
    colour2 = 0;
    colour3 = 0;
    colour4 = 0;
    colour5 = 0;
}
    
void ReadLives(uint16_t GPIO_Pin1, uint16_t GPIO_Pin2, uint16_t GPIO_Pin3, GPIO_PinState state_1,  GPIO_PinState state_2, GPIO_PinState state_3)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_Pin1, state_1);
    HAL_GPIO_WritePin(GPIOC, GPIO_Pin2, state_2);
    HAL_GPIO_WritePin(GPIOC, GPIO_Pin3, state_3);
}

void SetLives()
{
    switch (R)
    {
        case 0: 
            ReadLives(GPIO_PIN_8, GPIO_PIN_6, GPIO_PIN_5, 1, 1, 1);
            break;
        case 1:
            ReadLives(GPIO_PIN_8, GPIO_PIN_6, GPIO_PIN_5, 1, 1, 0);
            break;
        case 2:
            ReadLives(GPIO_PIN_8, GPIO_PIN_6, GPIO_PIN_5, 1, 0, 0);
            break;
        case 3:
            ReadLives(GPIO_PIN_8, GPIO_PIN_6, GPIO_PIN_5, 0, 0, 0);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);
            G++;
            break;
    }
}


void ReadLevels(uint16_t GPIO_Pin1, uint16_t GPIO_Pin2, uint16_t GPIO_Pin3, uint16_t GPIO_Pin4, uint16_t GPIO_Pin5, 
            GPIO_PinState state_1,  GPIO_PinState state_2, GPIO_PinState state_3, GPIO_PinState state_4, GPIO_PinState state_5)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_Pin1, state_1);
    HAL_GPIO_WritePin(GPIOB, GPIO_Pin2, state_2);
    HAL_GPIO_WritePin(GPIOB, GPIO_Pin3, state_3);
    HAL_GPIO_WritePin(GPIOB, GPIO_Pin4, state_4);
    HAL_GPIO_WritePin(GPIOB, GPIO_Pin5, state_5);
}
void SetLevels()
{
    switch (S)
    {
        case 0:
            ReadLevels(GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13, 0, 0, 0, 0, 0);
            break;
        case 1:
            ReadLevels(GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13, 1, 0, 0, 0, 0);
            break;
        case 2: 
            ReadLevels(GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13, 1, 1, 0, 0, 0);
            break;
        case 3: 
            ReadLevels(GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13, 1, 1, 1, 0, 0);
            break;
        case 4:
            ReadLevels(GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13, 1, 1, 1, 1, 0);
            break;
        case 5:
            ReadLevels(GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13, 1, 1, 1, 1, 1);
            break;
        case 6: 
            ReadLevels(GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_13, 1, 1, 1, 1, 1);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);
            G++;
            break;
    }
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}

