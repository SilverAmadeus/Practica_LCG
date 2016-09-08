#include "Main.h"
#include "3ds.h"
// TODO: Camara 2, hue
//Release se usa para "lanzar" nuestro proyecto y se pueda usar en cualquier maquina.

HDC			hDC=NULL;		// Dispositivo de contexto GDI
HGLRC		hRC=NULL;		// Contexto de renderizado
HWND		hWnd=NULL;		// Manejador de ventana
HINSTANCE	hInstance;		// Instancia de la aplicacion

bool	keys[256];			// Arreglo para el manejo de teclado
bool	active=TRUE;		// Bandera de ventana activa

int glWidth;
int glHeight;

int modoRender=1; //Solido: 1, Alambrado: 2

CVector posicionCamProv;
CVector objetivoCamProv;


//Apuntador para primitivas de cuadricas
GLUquadricObj	*e;

//Nombre y ubicación de los modelos
#define FILE_NAME1  "Modelos/modelo1.3ds"
#define FILE_NAME2  "Modelos/modelo2.3ds"
#define FILE_NAME3  "Modelos/modelo3.3ds"

//Contenedores de texturas de cada modelo
CTga textureModel1[4];
CTga textureModel2[4];
CTga textureModel3[4];

//Contenedor de texturas para el escenario
CTga textureName[20];

//Objeto que da acceso a las funciones del cargador 3ds
CLoad3DS g_Load3ds;

//Instancias de la estructura que almacenaran los datos de cada modelo
t3DModel g_3DModel1;
t3DModel g_3DModel2;
t3DModel g_3DModel3;

//Objeto para acceder a las variables de control del personaje
paramObjCam player1;

camara camara1;
camara camara2;
camara camara3;
int tipoCamara;


LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaracion de WndProc (Procedimiento de ventana)

GLvoid ReDimensionaEscenaGL(GLsizei width, GLsizei height)	// Redimensiona e inicializa la ventana
{
	if (height==0)							// Para que no se presente una division por cero
	{
		height=1;							// la altura se iguala a 1
	}

	glViewport(0,0,width,height);					// Resetea el puerto de vista

	glMatrixMode(GL_PROJECTION);					// Selecciona la Matriz de Proyeccion
	glLoadIdentity();								// Resetea la Matriz de Proyeccion

	// Calcula el radio de aspecto o proporcion de medidas de la ventana
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,2000.0f);
	
	glMatrixMode(GL_MODELVIEW);							// Selecciona la Matriz de Vista de Modelo
	glLoadIdentity();									// Resetea la Matriz de Vista de Modelo

	glWidth=width;
	glHeight=height;
}

int CargaModelos()
{
	if(!g_Load3ds.Load3DSFile(FILE_NAME1, &g_3DModel1, textureModel1))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME2, &g_3DModel2, textureModel2))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME3, &g_3DModel3, textureModel3))
		return 0;
	
	return TRUE;
}

void DescargaModelos()
{
	g_Load3ds.UnLoad3DSFile(&g_3DModel1, textureModel1);
	g_Load3ds.UnLoad3DSFile(&g_3DModel2, textureModel2);
	g_Load3ds.UnLoad3DSFile(&g_3DModel3, textureModel3);
}

// Todas las funciones que llegan alterar caracteristicas del personaje se definen antes

void iniciaCamara()
{
	tipoCamara = 1;

	//Cámara 1
	camara1.posCam = CVector(0.0f, 5.0f, 80.0f);
	camara1.objCam = CVector(0.0f, 5.0f, -20.0f);
	camara1.altCam = 5.0f;
	camara1.altObj = 5.0f;
	camara1.velCam = 0.5f;

	//Cámara 2
	camara2.distCam = 20.0f;
	camara2.velCam = 0.5f;
	camara2.angCam = 90.0f;

	camara2.posCam.y = 5.0f;

	camara2.objCam = CVector(0.0f, 5.0f, 0.0f);
	camara2.altCam = 5.0f;
	camara2.altObj = 5.0f;

	//Cámara 3
	camara3.distCam = 20.0f;
	camara3.velCam = 0.5f;
	camara3.angCam = 270.0f;
	camara3.posCam = CVector(0.0f, 5.0f, 0.0f);
	camara3.objCam = CVector(0.0f, 5.0f, -20.0f);
	camara2.dirCam.x = cos(camara2.angCam*PI / 180.0f);
	camara2.dirCam.y = 0.0f;
	camara2.dirCam.z = sin(camara2.angCam*PI / 180.0f);
	camara3.altCam = 5.0f;
	camara3.altObj = 5.0f;


        // Camara 4
}


int IniGL(GLvoid)										// Aqui se configuran los parametros iniciales de OpenGL
{
	glShadeModel(GL_SMOOTH);							// Activa Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Fondo negro, los primeros tres argumentos definen R/G/B el ultimo es transparencia (Alpha)
	// 1 es el valor maximo que puede tomar
	glClearDepth(1.0f);									// Valor para el Depth Buffer
	glEnable(GL_DEPTH_TEST);							// Activa Depth Testing
	glDepthFunc(GL_LEQUAL);								// Tipo de Depth Testing a usar
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Correccion de cálculos de perspectiva

	glCullFace(GL_BACK);								// Configurado para eliminar caras traseras
	glEnable(GL_CULL_FACE);								// Activa eliminacion de caras ocultas

	e=gluNewQuadric();
	
	posicionCamProv = CVector(0.0f, 10.0f, 80.0f);
	objetivoCamProv = CVector(0.0f, 10.0f, 0.0f);

	iniciaCamara();

	return TRUE;										
}

void controlCamara(int funcion)
{
	if (tipoCamara == 1)
	{
		if (funcion == 1) //Avanza hacia enfrente (z-)
		{
			camara1.posCam.z -= camara1.velCam;
			camara1.objCam.z -= camara1.velCam;
		}
		else if (funcion == 2) //Avanza hacia atrás (z+)
		{
			camara1.posCam.z += camara1.velCam;
			camara1.objCam.z += camara1.velCam;
		}
		else if (funcion == 3) //Avanza hacia la derecha (x+)
		{
			camara1.posCam.x += camara1.velCam;
			camara1.objCam.x += camara1.velCam;
		}
		else if (funcion == 4) //Avanza hacia la izquierda (x-)
		{
			camara1.posCam.x -= camara1.velCam;
			camara1.objCam.x -= camara1.velCam;
		}
		else if (funcion == 5) //Sube pos camara (y+)
		{
			camara1.posCam.y += camara1.velCam;
			camara1.altCam = camara1.posCam.y;
		}
		else if (funcion == 6) //Baja pos camara (y-)
		{
			camara1.posCam.y -= camara1.velCam;
			camara1.altCam = camara1.posCam.y;
		}
		else if (funcion == 7) //Sube obj camara (y+)
		{
			camara1.objCam.y += camara1.velCam;
			camara1.altObj = camara1.objCam.y;
		}
		else if (funcion == 8) //Baja obj camara (y-)
		{
			camara1.objCam.y -= camara1.velCam;
			camara1.altObj = camara1.objCam.y;
		}
	}
	else if (tipoCamara == 2)
	{
		if (funcion == 1) //Disminuye radio de la camara (distCam)
		{

		}
		else if (funcion == 2) // Aumenta radio de la camara (distCam)
		{

		}
		else if (funcion == 3) //Gira hacia la derecha
		{

		}
		else if (funcion == 4) //Gira hacia la izquierda
		{

		}
		else if (funcion == 5) //Sube pos camara (y+)
		{

		}
		else if (funcion == 6) //Baja pos camara (y-)
		{

		}
		else if (funcion == 7) //Sube obj camara (y+)
		{

		}
		else if (funcion == 8) //Baja obj camara (y-)
		{

		}

		//Cálculo de la posición y objetivo de la cámara

	}
	else if (tipoCamara == 3)
	{
		if (funcion == 1) //Avanza hacia enfrente
		{
			camara3.posCam = camara3.posCam + camara3.dirCam*camara3.velCam;
			camara3.posCam.y = camara3.altCam;
			camara3.objCam = camara3.posCam + camara3.dirCam*camara3.distCam;
			camara3.objCam.y = camara3.altObj;
		}
		else if (funcion == 2) //Avanza hacia atrás
		{
			camara3.posCam = camara3.posCam - camara3.dirCam*camara3.velCam;
			camara3.posCam.y = camara3.altCam;
			camara3.objCam = camara3.posCam + camara3.dirCam*camara3.velCam;
			camara3.objCam.y = camara3.altObj;

		}
		else if (funcion == 3) //Gira hacia la derecha
		{
			camara3.angCam += camara3.velCam;
			if (camara3.angCam > 360.0f) // Se resta para que no haya overflow cuando se deje presionada la tecla
				camara3.angCam -= 360.0f;

			camara3.dirCam.x = cos(camara3.angCam*PI / 180.0f);
			camara3.dirCam.z = sin(camara3.angCam*PI / 180.0f);
			camara3.dirCam.y = 0.0f;

			camara3.objCam = camara3.posCam + camara3.dirCam*camara3.distCam;
			camara3.objCam.y = camara3.altObj;
		}
		else if (funcion == 4) //Gira hacia la izquierda
		{
			camara3.angCam -= camara3.velCam;
			if (camara3.angCam < 360.0f) // Se  para que no haya overflow cuando se deje presionada la tecla
				camara3.angCam -= 360.0f;

			camara3.dirCam.x = cos(camara3.angCam*PI / 180.0f);
			camara3.dirCam.z = sin(camara3.angCam*PI / 180.0f);
			camara3.dirCam.y = 0.0f;

			camara3.objCam = camara3.posCam + camara3.dirCam*camara3.distCam;
			camara3.objCam.y = camara3.altObj;

		}
		else if (funcion == 5) //Sube pos camara (y+)
		{
                    camara3.posCam.y += camara3.velCam;
                    camara3.altCam = camara3.posCam.y;

		}
		else if (funcion == 6) //Baja pos camara (y-)
		{
			camara3.posCam.y -= camara3.velCam;
			camara3.altCam = camara3.posCam.y;
		}
		else if (funcion == 7) //Sube obj camara (y+)
		{
			camara3.objCam.y += camara3.velCam;
			camara3.altObj = camara3.objCam.y;
		}
		else if (funcion == 8) //Baja obj camara (y-)
		{
			camara3.objCam.y -= camara3.velCam;
			camara3.altObj = camara3.objCam.y;
		}
	}
}

void DibujaEjes()
{
	glBegin(GL_LINES);
		//Eje X
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);

		//Eje Y
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f,  100.0f, 0.0f);

		//Eje Z
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f,  100.0f);
	glEnd();

	glPointSize(10.0f);

	glBegin(GL_POINTS);
		//"Flecha" eje X
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);

		//"Flecha" eje Y
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,  100.0f, 0.0f);

		//"Flecha" eje Z
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f, 0.0f,  100.0f);
	glEnd();

	glPointSize(1.0f);

	glColor3f(1.0f,1.0f,1.0f);
}


void dibujaEscenario()
{

	int codigo;
	if(modoRender == 1) // Solido (codigo)
		codigo = 0x0007;
	if(modoRender == 2) // Alambrado (GL_LINE_LOOP)
		codigo = 0x0002;
	//objeto 1
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(codigo);
		glVertex3f(-120.0f, 0.0f, 17.0f);
		glVertex3f(  80.0f, 0.0f, 17.0f);
		glVertex3f(  80.0f, 0.0f,  0.0f);
		glVertex3f(-120.0f, 0.0f,  0.0f);
	glEnd();

	//Normal que se forma cuando el plano se genera no debe ser mayor a 90 grados para que lo pueda captar la camara
	//Normal de cuando se genera el plano debe apuntar a la camara para que el plano sea visible
	
	//glBegin(codigo);
	//	glVertex3f(  80.0f, 0.0f, 17.0f);
	//	glVertex3f(-120.0f, 0.0f, 17.0f);

	//	glVertex3f(-120.0f, 0.0f,  0.0f);
	//	glVertex3f(  80.0f, 0.0f,  0.0f);
	//glEnd();

	//objeto 2
	glColor3f(1.0f, 0.4f, 0.0f);
	
	glBegin(codigo);
		glVertex3f(-120.0f, 0.0f,  0.0f);
		glVertex3f(  50.0f, 0.0f,  0.0f);
		glVertex3f(  50.0f, 0.0f,-20.0f);
		glVertex3f(-120.0f, 0.0f,-20.0f);
	glEnd();

	//objeto 2a
	glColor3f(1.0f, 0.4f, 0.0f);
	
	glBegin(codigo);
		
		glVertex3f(50.0f, 0.0f,-12.0f);
		glVertex3f(80.0f, 0.0f,-12.0f);
		glVertex3f(80.0f, 0.0f,-30.0f);
		glVertex3f(50.0f, 0.0f,-30.0f);
	glEnd();

	//objeto 3
	glColor3f(1.0f, 0.8f, 0.0f);

	glBegin(codigo);
		
		glVertex3f(-115.0f, 0.0f,-20.0f);
		glVertex3f(- 50.0f, 0.0f,-20.0f);
		glVertex3f(- 50.0f, 0.0f,-40.0f);
		glVertex3f(-115.0f, 0.0f,-40.0f);
	glEnd();

	//objeto 4
	glColor3f(1.0f, 1.0f, 0.0f);

	glBegin(codigo);
		
		glVertex3f( 80.0f, 0.0f, 17.0f);
		glVertex3f(140.0f, 0.0f, 17.0f);
		glVertex3f(140.0f, 0.0f,-30.0f);
		glVertex3f( 80.0f, 0.0f,-30.0f);
	glEnd();

	//objeto 5
	glColor3f(1.0f, 1.0f, 0.2f);

	glBegin(codigo);
		
		glVertex3f(-110.0f,  0.0f,-40.0f);
		glVertex3f(- 85.0f,  0.0f,-40.0f);
		glVertex3f(- 85.0f, 17.0f,-40.0f);
		glVertex3f(-110.0f, 17.0f,-40.0f);
	glEnd();

	//objeto 6
	glColor3f(1.0f, 1.0f, 0.6f);

	glBegin(codigo);
		
		glVertex3f(-85.0f,  0.0f,-40.0f);
		glVertex3f(-60.0f,  0.0f,-40.0f);
		glVertex3f(-60.0f, 19.0f,-40.0f);
		glVertex3f(-85.0f, 19.0f,-40.0f);
	glEnd();

	//objeto 7 (2 planos)
	glColor3f(0.0f, 1.0f, 0.0f);

	glBegin(codigo);
		
		glVertex3f(-120.0f,  0.0f, 0.0f);
		glVertex3f(- 95.0f,  0.0f, 0.0f);
		glVertex3f(- 95.0f, 22.0f, 0.0f);
		glVertex3f(-120.0f, 22.0f, 0.0f);
	glEnd();

	glColor3f(0.7f, 1.0f, 0.0f);

	glBegin(codigo);
		
		glVertex3f(-95.0f,  0.0f,  0.0f);
		glVertex3f(-95.0f,  0.0f,-18.0f);
		glVertex3f(-95.0f, 22.0f,-18.0f);
		glVertex3f(-95.0f, 22.0f,  0.0f);
	glEnd();

	//objeto 8 (10 planos)
	//1
	glColor3f(0.0f, 1.0f, 0.4f);

	glBegin(codigo);
		
		glVertex3f(-78.0f,  0.0f, 0.0f);
		glVertex3f(-68.0f,  0.0f, 0.0f);
		glVertex3f(-68.0f, 22.0f, 0.0f);
		glVertex3f(-78.0f, 22.0f, 0.0f);
	glEnd();

	//2
	glColor3f(0.0f, 1.0f, 0.8f);
	
	glBegin(codigo);
		
		glVertex3f(-68.0f,  7.0f, 0.0f);
		glVertex3f(-63.0f,  7.0f, 0.0f);
		glVertex3f(-63.0f, 22.0f, 0.0f);
		glVertex3f(-68.0f, 22.0f, 0.0f);
	glEnd();

	//3
	glColor3f(1.0f, 1.0f, 0.5f);
	
	glBegin(codigo);
		
		glVertex3f(-68.0f, 0.0f, 0.0f);
		glVertex3f(-63.0f, 0.0f, 0.0f);
		glVertex3f(-63.0f, 1.0f, 0.0f);
		glVertex3f(-68.0f, 1.0f, 0.0f);
	glEnd();

	//4
	glColor3f(0.5f, 0.0f, 1.0f);
	
	glBegin(codigo);
		
		glVertex3f(-63.0f,  0.0f, 0.0f);
		glVertex3f(-10.0f,  0.0f, 0.0f);
		glVertex3f(-10.0f, 22.0f, 0.0f);
		glVertex3f(-63.0f, 22.0f, 0.0f);
	glEnd();

	//5
	glColor3f(0.5f, 0.4f, 1.0f);

	glBegin(codigo);
		
		glVertex3f(-10.0f,  0.0f,  0.0f);
		glVertex3f(-10.0f,  0.0f,-18.0f);
		glVertex3f(-10.0f, 22.0f,-18.0f);
		glVertex3f(-10.0f, 22.0f,  0.0f);
	glEnd();

	//6
	glColor3f(0.5f, 0.8f, 1.0f);

	glBegin(codigo);
		
		glVertex3f(-78.0f,  0.0f,-18.0f);
		glVertex3f(-78.0f,  0.0f,  0.0f);
		glVertex3f(-78.0f, 22.0f,  0.0f);
		glVertex3f(-78.0f, 22.0f,-18.0f);
	glEnd();

	//7
	glColor3f(1.0f, 0.2f, 0.5f);

	glBegin(codigo);
		
		glVertex3f(-68.0f, 1.0f, 0.0f);
		glVertex3f(-63.0f, 1.0f, 0.0f);
		glVertex3f(-63.0f, 1.0f,-5.0f);
		glVertex3f(-68.0f, 1.0f,-5.0f);
	glEnd();

	//8
	glColor3f(0.0f, 0.0f, 1.0f);
	
	glBegin(codigo);
		
		glVertex3f(-68.0f, 1.0f, 0.0f);
		glVertex3f(-68.0f, 1.0f,-5.0f);
		glVertex3f(-68.0f, 7.0f,-5.0f);
		glVertex3f(-68.0f, 7.0f, 0.0f);
	glEnd();

	//9
	glColor3f(0.0f, 0.2f, 1.0f);
	
	glBegin(codigo);
		
		glVertex3f(-63.0f, 1.0f,-5.0f);
		glVertex3f(-63.0f, 1.0f, 0.0f);
		glVertex3f(-63.0f, 7.0f, 0.0f);
		glVertex3f(-63.0f, 7.0f,-5.0f);
	glEnd();

	//9a
	glBegin(codigo);
		
		glVertex3f(-68.0f, 1.0f,-5.0f);
		glVertex3f(-63.0f, 1.0f,-5.0f);
		glVertex3f(-63.0f, 7.0f,-5.0f);
		glVertex3f(-68.0f, 7.0f,-5.0f);
	glEnd();

	//Let the chopping begin!
	//My shit starts here

	//10
	glBegin(codigo);
		glColor3f(0.11f, 0.97f, 0.88f);
		
		glVertex3f(-10.0f, 0.0f, -1.0f);
		glVertex3f( 50.0f, 0.0f, -1.0f);
		glVertex3f( 50.0f, 8.0f, -1.0f);
		glVertex3f(-10.0f, 8.0f, -1.0f);
	glEnd();

	//11 Shadow

	glBegin(codigo);
		glColor3f(0.08f, 0.32f, 0.67f);
		
		glVertex3f(50.0f, 0.0f,  0.0f);
		glVertex3f(50.0f, 22.0f, 0.0f);
		glVertex3f(50.0f, 22.0f, -12.0f);
		glVertex3f(50.0f, 0.0f, -12.0f);
	glEnd();
	
	//11 Front

	glBegin(codigo);
		glColor3f(0.5f, 0.73f, 0.95f);
		
		glVertex3f(50.0f, 0.0f,  0.0f);
		glVertex3f(56.0f, 0.0f, 0.0f);
		glVertex3f(56.0f, 22.0f, 0.0f);
		glVertex3f(50.0f, 22.0f, 0.0f);
	glEnd();

        //11_inside

        glBegin(codigo);
                glColor3f(0.0f, 0.29f, 0.29f);

                glVertex3f(56.0f, 1.0f, 0.0f);
                glVertex3f(56.0f, 1.0f, -5.0f);
                glVertex3f(56.0f, 7.0f, -5.0f);
                glVertex3f(56.0f, 7.0f, 0.0f);
        glEnd();

	//12 Front_a

	glBegin(codigo);
		glColor3f(0.92f, 0.80f, 0.91f);
		
		glVertex3f(56.0f, 0.0f,  0.0f);
		glVertex3f(62.0f, 0.0f, 0.0f);
		glVertex3f(62.0f, 1.0f, 0.0f);
		glVertex3f(56.0f, 1.0f, 0.0f);
	glEnd();

        //12 Front_a_Up_Cover

        glBegin(codigo);
                glColor3f(0.66f, 0.0f, 0.66f);

                glVertex3f(56.0f, 1.0f, 0.0f);
                glVertex3f(62.0f, 1.0f, 0.0f);
                glVertex3f(62.0f, 1.0f, -5.0f);
                glVertex3f(56.0f, 1.0f, -5.0f);
        glEnd();
	
	//12 Front_b

	glBegin(codigo);
		glColor3f(0.54f, 0.18f, 0.98f);
		
		glVertex3f(56.0f, 7.0f,  0.0f);
		glVertex3f(62.0f, 7.0f, 0.0f);
		glVertex3f(62.0f, 22.0f, 0.0f);
		glVertex3f(56.0f, 22.0f, 0.0f);
	glEnd();

	//13 Front

	glBegin(codigo);
		glColor3f(0.95f, 0.4f, 0.95f);
		
		glVertex3f(62.0f, 0.0f,  0.0f);
		glVertex3f(68.0f, 0.0f, 0.0f);
		glVertex3f(68.0f, 22.0f, 0.0f);
		glVertex3f(62.0f, 22.0f, 0.0f);
	glEnd();

        //13_inside_a

        glBegin(codigo);
                glColor3f(0.4f, 0.0f, 0.4f);

                glVertex3f(62.0f, 1.0f, 0.0f);
                glVertex3f(62.0f, 7.0f, 0.0f);
                glVertex3f(62.0f, 7.0f, -5.0f);
                glVertex3f(62.0f, 1.0f, -5.0f);
        glEnd();

        //13_inside_b

        glBegin(codigo);
                glColor3f(0.4f, 0.0f, 0.4f);

                glVertex3f(68.0f, 1.0f, -5.0f);
                glVertex3f(68.0f, 7.0f, -5.0f);
                glVertex3f(68.0f, 7.0f, 0.0f);
                glVertex3f(68.0f, 1.0f, 0.0f);
        glEnd();



	//14 Front_a

	glBegin(codigo);
		glColor3f(0.58f, 0.84f, 0.61f);
		
		glVertex3f(68.0f, 0.0f,  0.0f);
		glVertex3f(74.0f, 0.0f, 0.0f);
		glVertex3f(74.0f, 1.0f, 0.0f);
		glVertex3f(68.0f, 1.0f, 0.0f);
	glEnd();

        //14 Front_inside_cover

        glBegin(codigo);
                glColor3f(0.13f, 0.35f, 0.109f);

                glVertex3f(68.0f, 1.0f, 0.0f);
                glVertex3f(74.0f, 1.0f, 0.0f);
                glVertex3f(74.0f, 1.0f, -5.0f);
                glVertex3f(68.0f, 1.0f, -5.0f);
        glEnd();
	
	//14 Front_b

	glBegin(codigo);
		glColor3f(0.19f, 0.55f, 0.23f);
		
		glVertex3f(68.0f, 7.0f,  0.0f);
		glVertex3f(74.0f, 7.0f, 0.0f);
		glVertex3f(74.0f, 22.0f, 0.0f);
		glVertex3f(68.0f, 22.0f, 0.0f);
	glEnd();

        //15 _Front

        glBegin(codigo);
                glColor3f(1.0f, 0.50f, 0.25f);

                glVertex3f(74.0f, 0.0f, 0.0f);
                glVertex3f(80.0f, 0.0f, 0.0f);
                glVertex3f(80.0f, 22.0f, 0.0f);
                glVertex3f(74.0f, 22.0f, 0.0f);
        glEnd();

        //15_inside

        glBegin(codigo);
                glColor3f(0.49f, 0.247f, 0.0f);

                glVertex3f(74.0f, 1.0f, 0.0f);
                glVertex3f(74.0f, 7.0f, 0.0f);
                glVertex3f(74.0f, 7.0f, -5.0f);
                glVertex3f(74.0f, 1.0f, -5.0f);
        glEnd();



        //15_Shadow
        glBegin(codigo);
                glColor3f(0.784f, 0.411f, 0.015f);

                glVertex3f(80.0f, 0.0f, 0.0f);
                glVertex3f(80.0f, 0.0f, -12.0f);
                glVertex3f(80.0f, 22.0f, -12.0f);
                glVertex3f(80.0f, 22.0f, 0.0f);
        glEnd();

        //15_Box_inside_shadow
        glBegin(codigo);
                 glColor3f(0.45f, 0.45f, 0.45f);

                 glVertex3f(56.0f, 1.0f, -5.0f);
                 glVertex3f(74.0f, 1.0f, -5.0f);
                 glVertex3f(74.0f, 7.0f, -5.0f);
                 glVertex3f(56.0f, 7.0f, -5.0f);
        glEnd();

        //16_Utter_wall

        glBegin(codigo);
                glColor3f(0.4f, 0.4f, 0.0f);

                glVertex3f(140.0f, 0.0f, 17.0f);
                glVertex3f(140.0f, 22.0f, 17.0f);
                glVertex3f(140.0f, 22.0f, -30.0f);
                glVertex3f(140.0f, 0.0f, -30.0f);
        glEnd();

        //17_Background_Wall

        glBegin(codigo);
                glColor3f(0.23f, 0.403f, 0.615f);

                glVertex3f(68.0f, 0.0f, -30.0f);
                glVertex3f(140.0f, 0.0f, -30.0f);
                glVertex3f(140.0f, 22.0f, -30.0f);
                glVertex3f(68.0f, 22.0f, -30.0f);
        glEnd();

        //18_Fence
        glBegin(codigo);
                glColor3f(0.20f, 0.278f, 0.286f);

                glVertex3f(130.0f, 0.0f, 17.0f);
                glVertex3f(130.0f, 9.0f, 17.0f);
                glVertex3f(130.0f, 9.0f, -26.0f);
                glVertex3f(130.0f, 0.0f, -26.0f);
        glEnd();

        //19_Background_Fence_a
        glBegin(codigo);
                glColor3f(0.4f, 0.4f, 0.0f);
        
                glVertex3f(130.0f, 0.0f, -26.0f);
                glVertex3f(130.0f, 9.0f, -26.0f);
                glVertex3f(105.0f, 9.0f, -26.0f);
                glVertex3f(105.0f, 0.0f, -26.0f);
        glEnd();

        //19_Background_Fence_b
        glBegin(codigo);
        glColor3f(0.4f, 0.4f, 0.0f);

                glVertex3f(90.0f, 0.0f, -26.0f);
                glVertex3f(90.0f, 9.0f, -26.0f);
                glVertex3f(68.0f, 9.0f, -26.0f);
                glVertex3f(68.0f, 0.0f, -26.0f);
        glEnd();

        //20_Back_floor
        glBegin(codigo);
        glColor3f(1.0f, 0.65f, 0.0f);

                glVertex3f(50.0f, 0.0f, -20.0f);
                glVertex3f(50.0f, 0.0f, -30.0f);
                glVertex3f(-50.0f, 0.0f, -30.0f);
                glVertex3f(-50.0f, 0.0f, -20.0f);
        glEnd();

        //Building1_a


        glBegin(codigo);
        glColor3f(0.20f, 0.52f, 0.16f);

                glVertex3f(50.0f, 0.0f, -30.0f);
                glVertex3f(50.0f, 22.0f, -30.0f);
                glVertex3f(40.0f, 22.0f, -30.0f);
                glVertex3f(40.0f, 0.0f, -30.0f);
        glEnd();

        //Building1_b
        glBegin(codigo);
                glColor3f(0.26f, 0.67f, 0.21f);

                glVertex3f(40.0f, 0.0f, -30.0f);
                glVertex3f(40.0f, 22.0f, -30.0f);
                glVertex3f(40.0f, 22.0f, -34.0f);
                glVertex3f(40.0f, 0.0f, -34.0f);
        glEnd();

        //Building1_c
        glBegin(codigo);
                glColor3f(0.18f, 0.47f, 0.152f);

                glVertex3f(50.0f, 0.0f, -34.0f);
                glVertex3f(50.0f, 22.0f, -34.0f);
                glVertex3f(50.0f, 22.0f, -30.0f);
                glVertex3f(50.0f, 0.0f, -30.0f);
        glEnd();

        //Building1_Wall_a
        glBegin(codigo);
                glColor3f(0.145f, 0.478f, 0.447f);

                glVertex3f(60.0f, 0.0f, -34.0f);
                glVertex3f(60.0f, 20.0f, -34.0f);
                glVertex3f(50.0f, 20.0f, -34.0f);
                glVertex3f(50.0f, 0.0f, -34.0f);
        glEnd();

        //Building1_Wall_b
        glBegin(codigo);
                glColor3f(0.623f, 0.639f, 0.211f);

                glVertex3f(40.0f, 0.0f, -34.0f);
                glVertex3f(40.0f, 20.0f, -34.0f);
                glVertex3f(30.0f, 20.0f, -34.0f);
                glVertex3f(30.0f, 0.0f, -34.0f);
        glEnd();

        //Building1_Wall_c
        glBegin(codigo);
                glColor3f(0.98f, 0.658f, 0.541f);

                glVertex3f(30.0f, 0.0f, -34.0f);
                glVertex3f(30.0f, 20.0f, -34.0f);
                glVertex3f(30.0f, 20.0f, -46.0f);
                glVertex3f(30.0f, 0.0f, -46.0f);
        glEnd();

        //Building2_Wall_a
        glBegin(codigo);
                glColor3f(0.98f, 0.658f, 0.541f);

                glVertex3f(20.0f, 0.0f, -46.0f);
                glVertex3f(20.0f, 20.0f, -46.0f);
                glVertex3f(20.0f, 20.0f, -34.0f);
                glVertex3f(20.0f, 0.0f, -34.0f);
        glEnd();

        //Building2_Wall_b
        glBegin(codigo);
                glColor3f(0.133f, 0.658f, 0.501f);

                glVertex3f(20.0f, 0.0f, -34.0f);
                glVertex3f(20.0f, 20.0f, -34.0f);
                glVertex3f(10.0f, 20.0f, -34.0f);
                glVertex3f(10.0f, 0.0f, -34.0f);
        glEnd();

        //Building2_a


        glBegin(codigo);
                glColor3f(0.13f, 0.568f, 0.658f);

                glVertex3f(10.0f, 0.0f, -30.0f);
                glVertex3f(10.0f, 22.0f, -30.0f);
                glVertex3f(8.0f, 22.0f, -30.0f);
                glVertex3f(8.0f, 0.0f, -30.0f);
        glEnd();

        //Building2_b
        glBegin(codigo);
                glColor3f(0.26f, 0.67f, 0.21f);

                glVertex3f(8.0f, 0.0f, -30.0f);
                glVertex3f(8.0f, 22.0f, -30.0f);
                glVertex3f(8.0f, 22.0f, -34.0f);
                glVertex3f(8.0f, 0.0f, -34.0f);
        glEnd();

        //Building2_c
        glBegin(codigo);
                glColor3f(0.18f, 0.47f, 0.152f);

                glVertex3f(10.0f, 0.0f, -34.0f);
                glVertex3f(10.0f, 22.0f, -34.0f);
                glVertex3f(10.0f, 22.0f, -30.0f);
                glVertex3f(10.0f, 0.0f, -30.0f);
        glEnd();

        //Building3_a


        glBegin(codigo);
                glColor3f(0.13f, 0.568f, 0.658f);

                glVertex3f(10.0f, 0.0f, -30.0f);
                glVertex3f(10.0f, 22.0f, -30.0f);
                glVertex3f(8.0f, 22.0f, -30.0f);
                glVertex3f(8.0f, 0.0f, -30.0f);
        glEnd();

        //Building3_b
        glBegin(codigo);
                glColor3f(0.26f, 0.67f, 0.21f);

                glVertex3f(8.0f, 0.0f, -30.0f);
                glVertex3f(8.0f, 22.0f, -30.0f);
                glVertex3f(8.0f, 22.0f, -34.0f);
                glVertex3f(8.0f, 0.0f, -34.0f);
        glEnd();

        //Building3_c
        glBegin(codigo);
                glColor3f(0.18f, 0.47f, 0.152f);

                glVertex3f(10.0f, 0.0f, -34.0f);
                glVertex3f(10.0f, 22.0f, -34.0f);
                glVertex3f(10.0f, 22.0f, -30.0f);
                glVertex3f(10.0f, 0.0f, -30.0f);
        glEnd();


        //Building4_Wall_a
        glBegin(codigo);
                glColor3f(0.145f, 0.478f, 0.447f);

                glVertex3f(8.0f, 0.0f, -34.0f);
                glVertex3f(8.0f, 20.0f, -34.0f);
                glVertex3f(-2.0f, 20.0f, -34.0f);
                glVertex3f(-2.0f, 0.0f, -34.0f);
        glEnd();

        //Building4_a


        glBegin(codigo);
                glColor3f(0.13f, 0.568f, 0.658f);

                glVertex3f(-2.0f, 0.0f, -30.0f);
                glVertex3f(-2.0f, 22.0f, -30.0f);
                glVertex3f(-12.0f, 22.0f, -30.0f);
                glVertex3f(-12.0f, 0.0f, -30.0f);
        glEnd();

        //Building4_b
        glBegin(codigo);
                glColor3f(0.26f, 0.67f, 0.21f);

                glVertex3f(-12.0f, 0.0f, -30.0f);
                glVertex3f(-12.0f, 22.0f, -30.0f);
                glVertex3f(-12.0f, 22.0f, -34.0f);
                glVertex3f(-12.0f, 0.0f, -34.0f);
        glEnd();

        //Building4_c
        glBegin(codigo);
                glColor3f(0.18f, 0.47f, 0.152f);

                glVertex3f(-2.0f, 0.0f, -34.0f);
                glVertex3f(-2.0f, 22.0f, -34.0f);
                glVertex3f(-2.0f, 22.0f, -30.0f);
                glVertex3f(-2.0f, 0.0f, -30.0f);
        glEnd();


        //Building4_Wall_b
        glBegin(codigo);
            glColor3f(0.623f, 0.639f, 0.211f);

            glVertex3f(-12.0f, 0.0f, -34.0f);
            glVertex3f(-12.0f, 20.0f, -34.0f);
            glVertex3f(-22.0f, 20.0f, -34.0f);
            glVertex3f(-22.0f, 0.0f, -34.0f);
        glEnd();


        //Building5_a


        glBegin(codigo);
                glColor3f(0.13f, 0.568f, 0.658f);

                glVertex3f(-22.0f, 0.0f, -30.0f);
                glVertex3f(-22.0f, 22.0f, -30.0f);
                glVertex3f(-24.0f, 22.0f, -30.0f);
                glVertex3f(-24.0f, 0.0f, -30.0f);
        glEnd();

        //Building5_b
        glBegin(codigo);
                glColor3f(0.26f, 0.67f, 0.21f);

                glVertex3f(-24.0f, 0.0f, -30.0f);
                glVertex3f(-24.0f, 22.0f, -30.0f);
                glVertex3f(-24.0f, 22.0f, -34.0f);
                glVertex3f(-24.0f, 0.0f, -34.0f);
        glEnd();

        //Building5_c
        glBegin(codigo);
                glColor3f(0.18f, 0.47f, 0.152f);

                glVertex3f(-22.0f, 0.0f, -34.0f);
                glVertex3f(-22.0f, 22.0f, -34.0f);
                glVertex3f(-22.0f, 22.0f, -30.0f);
                glVertex3f(-22.0f, 0.0f, -30.0f);
        glEnd();


        //Building5_Wall_a
        glBegin(codigo);
                glColor3f(0.145f, 0.478f, 0.447f);

                glVertex3f(-24.0f, 0.0f, -34.0f);
                glVertex3f(-24.0f, 20.0f, -34.0f);
                glVertex3f(-34.0f, 20.0f, -34.0f);
                glVertex3f(-34.0f, 0.0f, -34.0f);
        glEnd();

        //Building6_a


        glBegin(codigo);
                glColor3f(0.13f, 0.568f, 0.658f);

                glVertex3f(-34.0f, 0.0f, -30.0f);
                glVertex3f(-34.0f, 22.0f, -30.0f);
                glVertex3f(-44.0f, 22.0f, -30.0f);
                glVertex3f(-44.0f, 0.0f, -30.0f);
        glEnd();

        //Building6_b
        glBegin(codigo);
                glColor3f(0.26f, 0.67f, 0.21f);

                glVertex3f(-44.0f, 0.0f, -30.0f);
                glVertex3f(-44.0f, 22.0f, -30.0f);
                glVertex3f(-44.0f, 22.0f, -34.0f);
                glVertex3f(-44.0f, 0.0f, -34.0f);
        glEnd();

        //Building6_c
        glBegin(codigo);
                glColor3f(0.18f, 0.47f, 0.152f);

                glVertex3f(-34.0f, 0.0f, -34.0f);
                glVertex3f(-34.0f, 22.0f, -34.0f);
                glVertex3f(-34.0f, 22.0f, -30.0f);
                glVertex3f(-34.0f, 0.0f, -30.0f);
        glEnd();

        //Weird_table_thing_a

        glBegin(codigo);
                glColor3f(0.27f, 0.48f, 0.521f);

                glVertex3f(-45.0f, 5.0f, 0.0f);
                glVertex3f(-55.0f, 5.0f, 0.0f);
                glVertex3f(-55.0f, 5.0f, 5.0f);
                glVertex3f(-45.0f, 5.0f, 5.0f);
        glEnd();

        //Weird_table_thing_b

        glBegin(codigo);
                glColor3f(0.16f, 0.29f, 0.30f);

                glVertex3f(-55.0f, 5.0f, 0.0f);
                glVertex3f(-55.0f, 2.0f, 0.0f);
                glVertex3f(-55.0f, 2.0f, 5.0f);
                glVertex3f(-55.0f, 5.0f, 5.0f);
        glEnd();

        //Weird_table_thing_c

        glBegin(codigo);
                glColor3f(0.16f, 0.29f, 0.30f);

                glVertex3f(-45.0f, 2.0f, 0.0f);
                glVertex3f(-45.0f, 5.0f, 0.0f);
                glVertex3f(-45.0f, 5.0f, 5.0f);
                glVertex3f(-45.0f, 2.0f, 5.0f);
        glEnd();

        //Weird_table_thing_d

        glBegin(codigo);
                glColor3f(0.443f, 0.52f, 0.27f);

                glVertex3f(-45.0f, 2.0f, 5.0f);
                glVertex3f(-45.0f, 5.0f, 5.0f);
                glVertex3f(-55.0f, 5.0f, 5.0f);
                glVertex3f(-55.0f, 2.0f, 5.0f);
        glEnd();

        //Weird_table_thing_e

        glBegin(codigo);
                glColor3f(0.16f, 0.25f, 0.23f);

                glVertex3f(-55.0f, 2.0f, 0.0f);
                glVertex3f(-54.0f, 1.0f, 1.0f);
                glVertex3f(-54.0f, 1.0f, 4.0f);
                glVertex3f(-55.0f, 2.0f, 5.0f);
        glEnd();

        //Weird_table_thing_f

        glBegin(codigo);
                glColor3f(0.50f, 0.32f, 0.458f);

                glVertex3f(-45.0f, 2.0f, 5.0f);
                glVertex3f(-55.0f, 2.0f, 5.0f);
                glVertex3f(-54.0f, 1.0f, 4.0f);
                glVertex3f(-46.0f, 1.0f, 4.0f);
        glEnd();

        //Weird_table_thing_g

        glBegin(codigo);
                glColor3f(0.16f, 0.25f, 0.23f);

                glVertex3f(-45.0f, 2.0f, 0.0f);
                glVertex3f(-45.0f, 2.0f, 5.0f);
                glVertex3f(-46.0f, 1.0f, 4.0f);
                glVertex3f(-46.0f, 1.0f, 1.0f);
        glEnd();

        //Weird_table_thing_h

        glBegin(codigo);
                glColor3f(0.5f, 1.0f, 0.623f);

                glVertex3f(-46.0f, 0.0f, 1.0f);
                glVertex3f(-46.0f, 1.0f, 1.0f);
                glVertex3f(-46.0f, 1.0f, 4.0f);
                glVertex3f(-46.0f, 0.0f, 4.0f);
        glEnd();

        //Weird_table_thing_i

        glBegin(codigo);
                glColor3f(0.694f, 0.176f, 0.80f);

                glVertex3f(-46.0f, 0.0f, 4.0f);
                glVertex3f(-46.0f, 1.0f, 4.0f);
                glVertex3f(-54.0f, 1.0f, 4.0f);
                glVertex3f(-54.0f, 0.0f, 4.0f);
        glEnd();

        //Weird_table_thing_j

        glBegin(codigo);
                glColor3f(0.5f, 1.0f, 0.623f);

                glVertex3f(-54.0f, 0.0f, 4.0f);
                glVertex3f(-54.0f, 1.0f, 4.0f);
                glVertex3f(-54.0f, 1.0f, 1.0f);
                glVertex3f(-54.0f, 0.0f, 1.0f);
        glEnd();

	//Al final de la función siempre se regresa al color blanco default de opengl
	glColor3f(1.0f,1.0f,1.0f);

}



int RenderizaEscena(GLvoid)								// Aqui se dibuja todo lo que aparecera en la ventana
{

	static char strBuffer[255] = { 0 };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Se limpian ambos buffers al iniciar el programa
	glLoadIdentity();									// Se limpian las matrices de transformaciones

	//gluLookAt(80.0f, 120.0f, 170.0f, 0.0f, 0.0f, 0.0f, 0, 1, 0);	//Primeros 3 parametros son los ejes X, Y, Z
																	//Ejes de objetivos, a lo que apunta el lente
	//gluLookAt(posicionCamProv.x, posicionCamProv.y, posicionCamProv.z, objetivoCamProv.x, objetivoCamProv.y, objetivoCamProv.z, 0, 1, 0);																	
	//Ultimos 3 parametros definen la posicion en la que rota
	//Cambios 30/08/16

	if (tipoCamara == 1)
		gluLookAt(camara1.posCam.x, camara1.posCam.y, camara1.posCam.z,
			camara1.objCam.x, camara1.objCam.y, camara1.objCam.z, 0, 1, 0);
	else if (tipoCamara == 2)
		gluLookAt(camara2.posCam.x, camara2.posCam.y, camara2.posCam.z,
			camara2.objCam.x, camara2.objCam.y, camara2.objCam.z, 0, 1, 0);
	else if (tipoCamara == 3)
		gluLookAt(camara3.posCam.x, camara3.posCam.y, camara3.posCam.z,
			camara3.objCam.x, camara3.objCam.y, camara3.objCam.z, 0, 1, 0);
	
	DibujaEjes();
	
	dibujaEscenario();

	sprintf(strBuffer, "Tipo cámara: %d", tipoCamara);
	SetWindowText(hWnd, strBuffer);
					
	return TRUE;
}

int RenderizaEscena2(GLvoid)								// Aqui se dibuja todo lo que aparecera en la ventana
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Se limpian ambos buffers

    for (int iNumViewPort = 0; iNumViewPort<4; iNumViewPort++) // La funcion toma el numero de puertos de vista en este caso son 2
    {
        if (iNumViewPort == 0)//Puerto 1, establece las dimensiones de las ventanas y puertos de vista
        {
            glViewport(0, glHeight*0.5f, glWidth*0.5f, glHeight*0.5f);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0f, (GLfloat)(glWidth*0.5f) / (GLfloat)glHeight, 1.0f, 2000.0f);
        }
        if (iNumViewPort == 1)//Puerto 2
        {
            glViewport(glWidth*0.5f, glHeight*0.5f, glWidth*0.5f, glHeight*0.5f);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0f, (GLfloat)(glWidth*0.5f) / (GLfloat)glHeight, 1.0f, 2000.0f);
        }
        if (iNumViewPort == 2)//Puerto 3
        {
            glViewport(0, 0, glWidth*0.5f, glHeight*0.5f);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0f, (GLfloat)(glWidth*0.5f) / (GLfloat)glHeight, 1.0f, 2000.0f);
        }
        if (iNumViewPort == 3)//Puerto 4
        {
            glViewport(glWidth*0.5f, 0, glWidth*0.5f, glHeight*0.5f);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0f, (GLfloat)(glWidth*0.5f) / (GLfloat)glHeight, 1.0f, 2000.0f);
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear(GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Se dibujan los puertos de vista

        if (iNumViewPort == 0) // Puerto de vista superior izquierdo
        {
            if (tipoCamara == 1)
                gluLookAt(camara1.posCam.x, camara1.posCam.y, camara1.posCam.z,
                    camara1.objCam.x, camara1.objCam.y, camara1.objCam.z, 0, 1, 0);
            else if (tipoCamara == 2)
                gluLookAt(camara2.posCam.x, camara2.posCam.y, camara2.posCam.z,
                    camara2.objCam.x, camara2.objCam.y, camara2.objCam.z, 0, 1, 0);
            else if (tipoCamara == 3)
                gluLookAt(camara3.posCam.x, camara3.posCam.y, camara3.posCam.z,
                    camara3.objCam.x, camara3.objCam.y, camara3.objCam.z, 0, 1, 0);

            DibujaEjes();
            modoRender = 1;
            dibujaEscenario();

        }

        if (iNumViewPort == 1) // Puerto de vista superior derecho
        {
            if (tipoCamara == 1)
                gluLookAt(camara1.posCam.x, camara1.posCam.y, camara1.posCam.z,
                    camara1.objCam.x, camara1.objCam.y, camara1.objCam.z, 0, 1, 0);
            else if (tipoCamara == 2)
                gluLookAt(camara2.posCam.x, camara2.posCam.y, camara2.posCam.z,
                    camara2.objCam.x, camara2.objCam.y, camara2.objCam.z, 0, 1, 0);
            else if (tipoCamara == 3)
                gluLookAt(camara3.posCam.x, camara3.posCam.y, camara3.posCam.z,
                    camara3.objCam.x, camara3.objCam.y, camara3.objCam.z, 0, 1, 0);

            DibujaEjes();
            modoRender = 2;

            dibujaEscenario();
        }
        if (iNumViewPort == 2) // Puerto de vista inferior izquierdo
        {
            gluLookAt(155, 50, 20, -25, 0, -15, 0, 1, 0);

            DibujaEjes();
            modoRender = 1;

            dibujaEscenario();
        }
        if (iNumViewPort == 3) // Puerto de vista inferior derecho
        {
            gluLookAt(0, 450, 0, 0, 0, 0, 0, 0, -1);

            DibujaEjes();
            modoRender = 2;

            dibujaEscenario();
            DibujaEjes();
            modoRender = 1;
            dibujaEscenario();
        }
    }

    return TRUE;
}


GLvoid DestruyeVentanaOGL(GLvoid)						// Elimina la ventana apropiadamente
{
	if (hRC)											// Si existe un contexto de renderizado...
	{
		if (!wglMakeCurrent(NULL,NULL))					// Si no se pueden liberar los contextos DC y RC...
		{
			MessageBox(NULL,"Falla al liberar DC y RC.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Si no se puede eliminar el RC?
		{
			MessageBox(NULL,"Falla al liberar el contexto de renderizado.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Se pone RC en NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Si no se puede eliminar el DC
	{
		MessageBox(NULL,"Falla al liberar el contexto de renderizado.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Se pone DC en NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Si no se puede destruir la ventana
	{
		MessageBox(NULL,"No se pudo liberar hWnd.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Se pone hWnd en NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Si no se puede eliminar el registro de la clase
	{
		MessageBox(NULL,"No se pudo eliminar el registro de la clase.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Se pone hInstance en NULL
	}
}

//	Este código crea la ventana de OpenGL.  Parámetros:					
//	title			- Titulo en la parte superior de la ventana			
//	width			- Ancho de la ventana								
//	height			- Alto de la ventana								
//	bits			- Número de bits a usar para el color (8/16/24/32)	
  
BOOL CreaVentanaOGL(char* title, int width, int height, int bits)
{
	GLuint	PixelFormat;				// Guarda el resultado despues de determinar el formato a usar
	WNDCLASS	wc;						// Estructura de la clase ventana
	DWORD		dwExStyle;				// Estilo extendido de ventana
	DWORD		dwStyle;				// Estilo de ventana
	RECT		WindowRect;				// Guarda los valores Superior Izquierdo / Inferior Derecho del rectángulo
	WindowRect.left=(long)0;			// Inicia el valor Izquierdo a 0
	WindowRect.right=(long)width;		// Inicia el valor Derecho al ancho especificado
	WindowRect.top=(long)0;				// Inicia el valor Superior a 0
	WindowRect.bottom=(long)height;		// Inicia el valor Inferior al alto especificado

	hInstance			= GetModuleHandle(NULL);				// Guarda una instancia de la ventana
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redibuja el contenido de la ventana al redimensionarla
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// Maneja los mensajes para WndProc
	wc.cbClsExtra		= 0;									// Ningun dato extra para la clase
	wc.cbWndExtra		= 0;									// Ningun dato extra para la ventana
	wc.hInstance		= hInstance;							// Inicia la instancia
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Carga el ícono por defecto
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Carga el puntero de flecha
	wc.hbrBackground	= NULL;									// No se requiere ningun fondo
	wc.lpszMenuName		= NULL;									// No hay menú en la ventana
	wc.lpszClassName	= "OpenGL";								// Fija el nombre de la clase.

	if (!RegisterClass(&wc))									// Intenta registrar la clase de ventana
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											
	}
		
	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;					// Estilo extendido de ventana
	dwStyle=WS_OVERLAPPEDWINDOW;									// Estilo de ventana

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Ajusta la ventana al tamaño especificado

	// Crea la ventana
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Estilo extendido para la ventana
								"OpenGL",							// Nombre de la clase
								title,								// Título de la ventana
								dwStyle |							// Definición del estilo de la ventana
								WS_CLIPSIBLINGS |					// Estilo requerido de la ventana
								WS_CLIPCHILDREN,					// Estilo requerido de la ventana
								0, 0,								// Posición de la ventana
								WindowRect.right-WindowRect.left,	// Calcula el ancho de la ventana
								WindowRect.bottom-WindowRect.top,	// Calcula el alto de la ventana
								NULL,								// No hay ventana superior
								NULL,								// No hay menú
								hInstance,							// Instancia
								NULL)))								// No se pasa nada a WM_CREATE
	{
		DestruyeVentanaOGL();										// Resetea el despliegue
		MessageBox(NULL,"Error al crear la ventana.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Si no se creo el contexto de dispositivo...
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede crear un contexto de dispositivo GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Si Windows no encontró un formato de pixel compatible
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede encontrar un formato de pixel compatible.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Si no se pudo habilitar el formato de pixel
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede usar el formato de pixel.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(hRC=wglCreateContext(hDC)))				// Si no se creo el contexto de renderizado
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede crear un contexto de renderizado GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!wglMakeCurrent(hDC,hRC))					// Si no se puede activar el contexto de renderizado
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede usar el contexto de renderizado GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	ShowWindow(hWnd,SW_SHOW);				// Muestra la ventana
	SetForegroundWindow(hWnd);				// Le da la prioridad mas alta
	SetFocus(hWnd);							// Pasa el foco del teclado a la ventana
	ReDimensionaEscenaGL(width, height);	// Inicia la perspectiva para la ventana OGL

	if (!IniGL())							// Si no se inicializa la ventana creada
	{
		DestruyeVentanaOGL();				// Resetea el despliegue
		MessageBox(NULL,"Falla en la inicialización.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	return TRUE;							// Todo correcto
}

LRESULT CALLBACK WndProc(	HWND	hWnd,	// Manejador para esta ventana
							UINT	uMsg,	// Mensaje para esta ventana
							WPARAM	wParam,	// Información adicional del mensaje
							LPARAM	lParam)	// Información adicional del mensaje
{
	switch (uMsg)							// Revisa los mensajes de la ventana
	{
		case WM_ACTIVATE:					// Revisa el mensaje de activación de ventana
		{
			if (!HIWORD(wParam))			// Revisa el estado de minimización
			{
				active=TRUE;				// El programa está activo
			}
			else
			{
				active=FALSE;				// El programa no está activo
			}

			return 0;						// Regresa al ciclo de mensajes
		}

		case WM_SYSCOMMAND:					// Intercepta comandos del sistema
		{
			switch (wParam)					// Revisa llamadas del sistema
			{
				case SC_SCREENSAVE:			// ¿Screensaver tratando de iniciar?
				case SC_MONITORPOWER:		// ¿Monitor tratando de entrar a modo de ahorro de energía?
				return 0;					// Evita que suceda
			}
			break;							// Sale del caso
		}

		case WM_CLOSE:						// Si se recibe un mensaje de cerrar...
		{
			PostQuitMessage(0);				// Se manda el mensaje de salida
			return 0;						// y se regresa al ciclo
		}

		case WM_KEYDOWN:					// Si se está presionando una tecla...
		{
			keys[wParam] = TRUE;			// Si es así, se marca como TRUE
			return 0;						// y se regresa al ciclo
		}

		case WM_KEYUP:						// ¿Se ha soltado una tecla?
		{
			keys[wParam] = FALSE;			// Si es así, se marca como FALSE
			return 0;						// y se regresa al ciclo
		}

		case WM_SIZE:						// Si se redimensiona la ventana...
		{
			ReDimensionaEscenaGL(LOWORD(lParam),HIWORD(lParam));  	// LoWord=Width, HiWord=Height
			return 0;						// y se regresa al ciclo
		}
	}

	// Pasa todos los mensajes no considerados a DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

// Este es el punto de entrada al programa; la función principal 
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instancia
					HINSTANCE	hPrevInstance,		// Instancia previa
					LPSTR		lpCmdLine,			// Parametros de la linea de comandos
					int			nCmdShow)			// Muestra el estado de la ventana
{
	MSG		msg;									// Estructura de mensajes de la ventana
	BOOL	done=FALSE;								// Variable booleana para salir del ciclo

	// Crea la ventana OpenGL
	if (!CreaVentanaOGL("Laboratorio de Computación Gráfica",640,480,16))
	{
		return 0;									// Salir del programa si la ventana no fue creada
	}

	while(!done)									// Mientras done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Revisa si hay mensajes en espera
		{
			if (msg.message==WM_QUIT)				// Si se ha recibido el mensje de salir...
			{
				done=TRUE;							// Entonces done=TRUE
			}
			else									// Si no, Procesa los mensajes de la ventana
			{
				TranslateMessage(&msg);				// Traduce el mensaje
				DispatchMessage(&msg);				// Envia el mensaje
			}
		}
		else										// Si no hay mensajes...
		{
			// Dibuja la escena. 
			if (active)								// Si está activo el programa...
			{
				if (keys[VK_ESCAPE])				// Si se ha presionado ESC
				{
					done=TRUE;						// ESC indica el termino del programa
				}
				else								// De lo contrario, actualiza la pantalla
				{
					RenderizaEscena2();				// Dibuja la escena
					SwapBuffers(hDC);				// Intercambia los Buffers (Double Buffering)
				}

				if(!ManejaTeclado()) return 0;
			}
			
		}
	}

	// Finalización del programa
	DestruyeVentanaOGL();							// Destruye la ventana
	return (msg.wParam);							// Sale del programa
}

// Agregar variables para cambiar de lugar la camara
int ManejaTeclado()
{
	//if(keys[VK_UP])
	//{		
	//	posicionCamProv.z-=0.5;
	//}
	//if(keys[VK_DOWN])
	//{
	//	posicionCamProv.z+=0.5f;
	//}
	//if(keys[VK_LEFT])
	//{
	//	posicionCamProv.x-=0.5f;
	//}
	//if(keys[VK_RIGHT])
	//{
	//	posicionCamProv.x+=0.5f;

	//}

	//if(keys[VK_PRIOR])
	//{
	//	posicionCamProv.y+=0.5f;
	//}
	//if(keys[VK_NEXT])
	//{
	//	posicionCamProv.y-=0.5f;
	//}

	//if(keys[VK_HOME])
	//{
	//	modoRender=1;
	//}

	//if(keys[VK_END])
	//{
	//	modoRender=2;
	//}

	//if(keys['A'])
	//{
	//	objetivoCamProv.x-=0.5f;
	//}
	//if(keys['S'])
	//{
	//	objetivoCamProv.z+=0.5f;
	//}
	//	if(keys['D'])
	//{
	//	objetivoCamProv.x+=0.5f;
	//}
	//	if(keys['W'])
	//{
	//	objetivoCamProv.z-=0.5f;
	//}
	//	if(keys['Q'])
	//{
	//	objetivoCamProv.y+=0.5f;
	//}
	//	if(keys['E'])
	//{
	//	objetivoCamProv.y-=0.5f;
	//}

	if (keys[VK_UP])
	{
		controlCamara(1);
	}
	if (keys[VK_DOWN])
	{
		controlCamara(2);
	}
	if (keys[VK_LEFT])
	{
		controlCamara(4);
	}
	if (keys[VK_RIGHT])
	{
		controlCamara(3);
	}

	if (keys[VK_PRIOR])
	{
		controlCamara(5);
	}
	if (keys[VK_NEXT])
	{
		controlCamara(6);
	}

	if (keys[VK_HOME])
	{
		controlCamara(7);
	}

	if (keys[VK_END])
	{
		controlCamara(8);
	}

	if (keys['A'])
	{
		tipoCamara = 1;
	}
	if (keys['S'])
	{
		tipoCamara = 2;


	}

	if (keys['D'])
	{
		tipoCamara = 3;

		camara3.dirCam.x = cos(camara3.angCam*PI / 180.0f);
		camara3.dirCam.z = sin(camara3.angCam*PI / 180.0f);
		camara3.dirCam.y = 0.0f;

		camara3.objCam = camara3.posCam + camara3.dirCam*camara3.distCam;
		camara3.objCam.y = camara3.altObj;

	}


	return TRUE;
}