#include "ProcessScanner.h"

void ds4_example();
int main(void) {
    ds4_example();
}

struct rgb { UCHAR r, g, b; };
int slowly_change_color(rgb& Ncolor, ProcessScanner& ps, UINT64 address);
void ds4_example() 
{

    ProcessScanner ps;
    ps.Init("DS4Windows");

    UCHAR pattern[11] = { 65,00,00,00,00,00,00,00,00,00,0xff };
    //41 00 00 00 00 00 00 00 00 00 FF  is a Very simple pattern which in the current version of DS4 will give you the address for the 
    //                                  colors of the controlers light when it is set to R=65 G=0 B=0 



    UINT64 address = ps.Find_pattern(pattern, sizeof(pattern));
    if (address) {
        std::cout << "FOUND THE COLORS ADDRESS, STARTING THE COLOR CHANGING \n";
    }
    else {
        std::cout << "SOME THING WENT WRONG CHECKOUT IF DS4 IS OPENED AND CONECTED TO YOUR CONTROLER USING THE WRITE COLORS \n";
    }
    rgb color{ 00,00,00 };


    while (true)
    {
        color.r = 255;
        slowly_change_color(color, ps, address);
        color.r = 0;

        color.g = 255;
        slowly_change_color(color, ps, address);
        color.g = 0;

        color.b = 255;
        slowly_change_color(color, ps, address);
        color.b = 0;
    }
}
int slowly_change_color(rgb &Ncolor, ProcessScanner &ps, UINT64 address)
{
    rgb Ocolor;
    ps.Read_memory_address(address, &Ocolor, sizeof(Ocolor));


    for (int i = 0; i <= 255; i++) 
    {
        Sleep(1);
        Ocolor.r += (Ncolor.r > Ocolor.r);
        Ocolor.g += (Ncolor.g > Ocolor.g);
        Ocolor.b += (Ncolor.b > Ocolor.b);

        Ocolor.r -= (Ncolor.r < Ocolor.r);
        Ocolor.g -= (Ncolor.g < Ocolor.g);
        Ocolor.b -= (Ncolor.b < Ocolor.b);

        ps.Write_memory_address(address, &Ocolor, sizeof(Ocolor));

    }
    return 0;
}
