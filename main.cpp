#include<windows.h>//Contains files for windows application
#include"stdio.h"//Contains files for command prompt input and output
#include"math.h"//Contains maths functions
#define CenterX 500
#define CenterY 400//Center coordinates of the screen
#define pi 3.1415926535898

int radius,cranklength;
struct entry{
       float degrees;
       float displacement;
};
entry Table[35];

void calculator()
{
     for(int n=0;n<360;n=n+10)
     {
             Table[n/10].degrees=n;
             Table[n/10].displacement=radius*cos((float)n*pi/180)+sqrt(cranklength*cranklength-radius*radius*pow(sin((float)n*pi/180),2));
     }
}        
/*
  So, what I did in the above code?
  The function is periodic. It repeats values over 360 degrees. So, we were going to calculate values every time?
  To avoid calculations again and again, I simply created a table, with angular and linear displacements.
  Note that calculations are CPU-intensive, while looking for data in a table is very fast.
  So it helps in making the program faster too.
  
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    static HDC hdc;
    static HPEN crankpen,conpen,cleanerpen,pathpen;
    static HBRUSH crankbrsh,conbrsh,cleanerbrsh;
    static int currentone=0;
    /*
      A quick summary on how graphics work in windows.
      A device context is a structure that holds data when doing graphics. It's advantage is that it works on all sorts of data.
      Use the same functions, irrespective of it's the screen, the printer, a saved image file, a picture in memory.
      Every DC can hold a pen(draws outlines), a brush(fill inside the outlines) and some other stuff too.
      A handle is like a handle to a car; you do not control everything manually in a car, you just control it's handle and the handle does the rest.
      This is why it is called a "handle to something".
    */
	switch(Message) {
		case WM_CREATE:// Runs when the program starts; first time initializations
             SetTimer(hwnd,1,100,0);//Set timer to interrupt every 100 milliseconds, that is 10 fps.
             hdc=GetDC(hwnd);//screen handle; draw here.
             conpen=CreatePen(PS_SOLID,4,RGB(255,0,0));
             crankpen=CreatePen(PS_SOLID,4,RGB(0,0,0));
             pathpen=CreatePen(PS_DOT,0,RGB(0,0,0));
             crankbrsh=CreateSolidBrush(RGB(0,0,0));
             conbrsh=CreateSolidBrush(RGB(255,0,0));
             cleanerpen=CreatePen(PS_SOLID,1,RGB(255,255,255));
             cleanerbrsh=CreateSolidBrush(RGB(255,255,255));
             break;
		
		case WM_TIMER://called when timer runs over, in our case every 100 milliseconds
             
             //Select a white pen and brush, and draw a huge rectangle. It whitens th screen and erases everything.
             SelectObject(hdc,cleanerpen);
             SelectObject(hdc,cleanerbrsh);
             Rectangle(hdc,0,0,8000,8000);
                          
             //Create the path of the rotation, a dotted circle.
             SelectObject(hdc,pathpen);
             SelectObject(hdc,cleanerbrsh);
             Ellipse(hdc,CenterX-20*radius,CenterY-20*radius,CenterX+20*radius,CenterY+20*radius);
             
             //Make the crank lever
             SelectObject(hdc,crankpen);
             SelectObject(hdc,crankbrsh);
             MoveToEx(hdc,CenterX,CenterY,0);
             LineTo(hdc,CenterX+20*radius*cos(Table[currentone].degrees*pi/180),CenterY-20*radius*sin(Table[currentone].degrees*pi/180));
             //Ellipse(hdc,CenterX-20*radius,CenterY-20*radius,CenterX+20*radius,CenterY+20*radius);
             
             //Draw the connecting rod
             SelectObject(hdc,conpen);
             SelectObject(hdc,conbrsh);
             //MoveToEx(hdc,CenterX+20*radius*cos(Table[currentone].degrees*pi/180),CenterY-20*radius*sin(Table[currentone].degrees*pi/180),0);
             LineTo(hdc,CenterX+20*radius+20*Table[currentone].displacement,CenterY);
             /The piston head
             Rectangle(hdc,CenterX+20*Table[currentone].displacement,CenterY-20*radius,CenterX+40*radius+20*Table[currentone].displacement,CenterY+20*radius);
             
             SelectObject(hdc,pathpen);
             SelectObject(hdc,cleanerbrsh);
             Ellipse(hdc, CenterX-20*radius, CenterY-60*radius, CenterX+20*radius, CenterY-20*radius);
             SelectObject(hdc, crankpen);
             SelectObject(hdc, crankbrsh);
             MoveToEx(hdc, CenterX, CenterY-40*radius, 0);
             LineTo(hdc,CenterX+20*radius*cos(Table[currentone].degrees*pi/180),CenterY-20*radius*sin(Table[currentone].degrees*pi/180)-40*radius);
             SelectObject(hdc, conpen);
             SelectObject(hdc, conbrsh);
             LineTo(hdc, CenterX+20*radius*cos(Table[currentone].degrees*pi/180)+20*cranklength, CenterY-20*radius*sin(Table[currentone].degrees*pi/180)-40*radius);
             Rectangle(hdc, CenterX+20*radius*cos(Table[currentone].degrees*pi/180)+20*cranklength, CenterY-60*radius, CenterX+20*radius*cos(Table[currentone].degrees*pi/180)+20*cranklength+40*radius,CenterY-20*radius);
             
             //Print the current displacement and angle of rotation
             printf("\t%3d\t%2.3f\n",(int)Table[currentone].degrees,Table[currentone].displacement);
             
             //Since 360 degrees is 0 degrees, so get it back to 0 once it crosses 35
             currentone++;
             if(currentone>35)currentone=0;
             break;
             
        case WM_DESTROY: //Called when the program is shutting down, when we press the close button.
             KillTimer(hwnd,1);
             DeleteObject(conpen);
             DeleteObject(conbrsh);
             DeleteObject(crankpen);
             DeleteObject(crankbrsh);
             ReleaseDC(hwnd,hdc);
			PostQuitMessage(0);
			break;
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
    do{
            printf("Enter the crank radius in cm:");
            scanf("%d",&radius);
            printf("\nEnter the connecting rod length in cm(must be greater than diameter of crank):");
            scanf("%d",&cranklength);
      }while(cranklength<2*radius);
    calculator();//Generate the table before the simulation starts.
    
    WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG Msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Slider-crank by 2019-ME-52",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		0, /* x */
		0, /* y */
		8000, /* width */
		8000, /* height */
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&Msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&Msg); /* Translate key codes to chars if present */
		DispatchMessage(&Msg); /* Send it to WndProc */
	}
	return Msg.wParam;
}
