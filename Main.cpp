#include "Main.h"
#include "3ds.h"
//Camara 3 fixed :)
// TODO: Escalera of d00m
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

//Nombre y ubicaci�n de los modelos
#define FILE_NAME1  "Modelos/modelo1.3ds"
#define FILE_NAME2  "Modelos/modelo2.3ds"
#define FILE_NAME3  "Modelos/modelo3.3ds"

#define FILE_NAME1g  "Modelos/torso.3ds"
#define FILE_NAME2g  "Modelos/cabeza.3ds"
#define FILE_NAME3g  "Modelos/piernader.3ds"
#define FILE_NAME4g  "Modelos/piernaizq.3ds"
#define FILE_NAME5g  "Modelos/brazoder.3ds"
#define FILE_NAME6g  "Modelos/brazoizq.3ds"
#define FILE_NAME7g  "Modelos/pierna_b.3ds"
#define FILE_NAME8g  "Modelos/tabla.3ds"


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

CTga textureModel1g[5];
CTga textureModel2g[5];
CTga textureModel3g[5];
CTga textureModel4g[5];
CTga textureModel5g[5];
CTga textureModel6g[5];
CTga textureModel7g[5];
CTga textureModel8g[5];

t3DModel g_3DModel1g;
t3DModel g_3DModel2g;
t3DModel g_3DModel3g;
t3DModel g_3DModel4g;
t3DModel g_3DModel5g;
t3DModel g_3DModel6g;
t3DModel g_3DModel7g;
t3DModel g_3DModel8g;


//Objeto para acceder a las variables de control del personaje
paramObjCam player1;

camara camara1;
camara camara2;
camara camara3;

int tipoCamara;

float Angt1;  //Rot. en x
float Angt2;  //Rot. en y
float Angc1;  //Rot. en x
float Angc2;  //Rot. en y

//Brazo izquierdo
float Angbi1;  //Rot. en X
float Angbi2;  //Rot. en Y
float Angbi3;  //Rot. en Z

//Brazo derecho
float Angbd1;
float Angbd2;
float Angbd3;

//Upper leg
float Angpizq; //Rot en X
float Angpder; //Rot en X

//Lower leg
float Angpi;
float Angpd;

float Xtor;
float Ytor;
float Ztor;

int IncDec;

//Variables para iluminacion
GLfloat LightPos[] = { 0.0f, 80.0f, 100.0f, 1.0f };		// Posici�n de la luz
GLfloat LightAmb[] = { 0.8f,  0.8f, 0.8f, 1.0f };		// Valores de la componente ambiente
GLfloat LightDif[] = { 0.9f,  0.9f, 0.9f, 1.0f };		// Valores de la componente difusa
GLfloat LightSpc[] = { 0.9f,  0.9f, 0.9f, 1.0f };		// Valores de la componente especular
CVector lightPosition;

GLfloat gDefaultAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat gDefaultDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat gDefaultSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat gDefaultShininess = 0.0f;

//Naranja metalico
GLfloat nmetalAmb[] = { 0.3f, 0.2f, 0.1f, 1.0f };
GLfloat nmetalDif[] = { 0.7f, 0.4f, 0.1f, 1.0f };
GLfloat nmetalSpe[] = { 0.6f, 0.3f, 0.1f, 1.0f };
GLfloat nmetalShi = 110.0f;

//Verde metalico
GLfloat vmetalAmb[] = { 0.1f, 0.3f, 0.14f, 1.0f };
GLfloat vmetalDif[] = { 0.08f, 0.7f, 0.11f, 1.0f };
GLfloat vmetalSpe[] = { 0.08f, 0.6f, 0.16f, 1.0f };
GLfloat vmetalShi = 110.0f;

//Animacion
const int maxKF = 9; //Num. total de KeyFrames para la animacion
FRAME KeyFrame[maxKF]; //Contenedor para almacenar cada keyframe de la animacion

bool play = false;
int playIndex = 0;

//En seccion de globales:
datosColadera datColadera;


//Contenedor de texturas para el escenario
CTga textura[25];

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

void CargaTexturas()
{
	textura[0].LoadTGA("Texturas/t1.tga");
	textura[1].LoadTGA("Texturas/t2.tga");
	textura[2].LoadTGA("Texturas/t3.tga");
	textura[3].LoadTGA("Texturas/t4.tga");
	textura[4].LoadTGA("Texturas/t5.tga");
	textura[5].LoadTGA("Texturas/t6.tga");
	textura[6].LoadTGA("Texturas/t7.tga");
	textura[7].LoadTGA("Texturas/t8.tga");
	textura[8].LoadTGA("Texturas/t9.tga");
	textura[9].LoadTGA("Texturas/t10.tga");
	textura[10].LoadTGA("Texturas/t11.tga");
	textura[11].LoadTGA("Texturas/t12.tga");
	textura[12].LoadTGA("Texturas/t13.tga");
	textura[13].LoadTGA("Texturas/t14.tga");
	textura[14].LoadTGA("Texturas/t15.tga");
	textura[15].LoadTGA("Texturas/t16.tga");
	textura[16].LoadTGA("Texturas/t17.tga");
	textura[17].LoadTGA("Texturas/t18.tga");
	textura[18].LoadTGA("Texturas/t19.tga");
	textura[19].LoadTGA("Texturas/t20.tga");
	textura[20].LoadTGA("Texturas/t21.tga");
}

void DescargaTexturas()
{
	textura[0].Elimina();
	textura[1].Elimina();
	textura[2].Elimina();
	textura[3].Elimina();
	textura[4].Elimina();
	textura[5].Elimina();
	textura[6].Elimina();
	textura[7].Elimina();
	textura[8].Elimina();
	textura[9].Elimina();
	textura[10].Elimina();
	textura[11].Elimina();
	textura[12].Elimina();
	textura[13].Elimina();
	textura[14].Elimina();
	textura[15].Elimina();
	textura[16].Elimina();
	textura[17].Elimina();
	textura[18].Elimina();
	textura[19].Elimina();
	textura[20].Elimina();
}


int CargaModelos()
{
    //Gaspar
    if (!g_Load3ds.Load3DSFile(FILE_NAME1g, &g_3DModel1g, textureModel1g))
        return 0;
    if (!g_Load3ds.Load3DSFile(FILE_NAME2g, &g_3DModel2g, textureModel2g))
        return 0;
    if (!g_Load3ds.Load3DSFile(FILE_NAME3g, &g_3DModel3g, textureModel3g))
        return 0;
    if (!g_Load3ds.Load3DSFile(FILE_NAME4g, &g_3DModel4g, textureModel4g))
        return 0;
    if (!g_Load3ds.Load3DSFile(FILE_NAME5g, &g_3DModel5g, textureModel5g))
        return 0;
    if (!g_Load3ds.Load3DSFile(FILE_NAME6g, &g_3DModel6g, textureModel6g))
        return 0;
    if (!g_Load3ds.Load3DSFile(FILE_NAME7g, &g_3DModel7g, textureModel7g))
        return 0;
    if (!g_Load3ds.Load3DSFile(FILE_NAME8g, &g_3DModel8g, textureModel8g))
        return 0;


    return TRUE;
}

void DescargaModelos()
{
    g_Load3ds.UnLoad3DSFile(&g_3DModel1g, textureModel1g); //Torso
    g_Load3ds.UnLoad3DSFile(&g_3DModel2g, textureModel2g);//Cabeza
    g_Load3ds.UnLoad3DSFile(&g_3DModel3g, textureModel3g);//Up leg right
    g_Load3ds.UnLoad3DSFile(&g_3DModel4g, textureModel4g); //Up leg left
    g_Load3ds.UnLoad3DSFile(&g_3DModel5g, textureModel5g);//Brazo derecho
    g_Load3ds.UnLoad3DSFile(&g_3DModel6g, textureModel6g);//Brazo izq
    g_Load3ds.UnLoad3DSFile(&g_3DModel7g, textureModel7g); //Low lew
    g_Load3ds.UnLoad3DSFile(&g_3DModel8g, textureModel8g); //Tabla

}


// Todas las funciones que llegan alterar caracteristicas del personaje se definen antes

void iniciaCamara()
{
	tipoCamara = 1;

	//C�mara 1
	camara1.posCam = CVector(0.0f, 5.0f, 80.0f);
	camara1.objCam = CVector(0.0f, 5.0f, -20.0f);
	camara1.altCam = 5.0f;
	camara1.altObj = 5.0f;
	camara1.velCam = 0.5f;

	//C�mara 2
	camara2.distCam = 20.0f;
	camara2.velCam = 0.5f;
	camara2.angCam = 90.0f;
        camara2.dirCam.x = cos(camara2.angCam*PI / 180.0f);
	camara2.dirCam.y = 0.0f;
        camara2.dirCam.z = sin(camara2.angCam*PI / 180.0f);
	camara2.altCam = 5.0f;
	camara2.altObj = 5.0f;
        camara2.posCam = CVector(camara2.dirCam.x*camara2.distCam, 8.0f, camara2.dirCam.z*camara2.distCam);
        camara2.objCam = CVector(0.0f, 8.0f, 0.0f);

	//C�mara 3
	camara3.distCam = 20.0f;
	camara3.velCam = 0.5f;
	camara3.angCam = 270.0f;
	camara3.posCam = CVector(0.0f, 5.0f, 10.0f);
	camara3.objCam = CVector(5.0f, 5.0f, -20.0f);
	camara3.dirCam.x = cos(camara2.angCam*PI / 180.0f);
	camara3.dirCam.y = 0.0f;
	camara3.dirCam.z = sin(camara2.angCam*PI / 180.0f);
	camara3.altCam = 5.0f;
	camara3.altObj = 5.0f;

}

void InicializaAnim()
{
	//Se inicializan las variables de la secuencia 1
	for (int i = 0; i<maxKF; i++)
	{
		KeyFrame[i].Angt1 = 0.0f;
		KeyFrame[i].Angt2 = 0.0f;
		KeyFrame[i].Angc1 = 0.0f;
		KeyFrame[i].Angc2 = 0.0f;
		KeyFrame[i].Angbi1 = 0.0f;
		KeyFrame[i].Angbi2 = 0.0f;
		KeyFrame[i].Angbib = 0.0f;
		KeyFrame[i].Angbd1 = 0.0f;
		KeyFrame[i].Angbd2 = 0.0f;
		KeyFrame[i].Angbdb = 0.0f;
		KeyFrame[i].Angpizq = 0.0f;
		KeyFrame[i].Angpder = 0.0f;
		KeyFrame[i].Angpi = 0.0f;
		KeyFrame[i].Angpd = 0.0f;
		KeyFrame[i].Xtor = 0.0f;
		KeyFrame[i].Ytor = 0.0f;
		KeyFrame[i].Ztor = 0.0f;

		KeyFrame[i].incAngt1 = 0.0f;
		KeyFrame[i].incAngt1 = 0.0f;
		KeyFrame[i].incAngc1 = 0.0f;
		KeyFrame[i].incAngc2 = 0.0f;
		KeyFrame[i].incAngbi1 = 0.0f;
		KeyFrame[i].incAngbi2 = 0.0f;
		KeyFrame[i].incAngbib = 0.0f;
		KeyFrame[i].incAngbd1 = 0.0f;
		KeyFrame[i].incAngbd2 = 0.0f;
		KeyFrame[i].incAngbdb = 0.0f;
		KeyFrame[i].incAngpizq = 0.0f;
		KeyFrame[i].incAngpder = 0.0f;
		KeyFrame[i].incAngpi = 0.0f;
		KeyFrame[i].incAngpd = 0.0f;
		KeyFrame[i].incXtor = 0.0f;
		KeyFrame[i].incYtor = 0.0f;
		KeyFrame[i].incZtor = 0.0f;
	}

	play = false;
	playIndex = 0;
}

void DatosAnimacion()
{
	//KF1
	KeyFrame[0].Angt1 = 0.0f;
	KeyFrame[0].Angt2 = 0.0f;
	KeyFrame[0].Angc1 = 20.0f;
	KeyFrame[0].Angc2 = 0.0f;
	KeyFrame[0].Angbi1 = 0.0f;
	KeyFrame[0].Angbi2 = 0.0f;
	KeyFrame[0].Angbib = 0.0f;
	KeyFrame[0].Angbd1 = 0.0f;
	KeyFrame[0].Angbd2 = 0.0f;
	KeyFrame[0].Angbdb = 0.0f;
	KeyFrame[0].Angpizq = 0.0f;
	KeyFrame[0].Angpder = 0.0f;
	KeyFrame[0].Angpi = 0.0f;
	KeyFrame[0].Angpd = 0.0f;
	KeyFrame[0].Xtor = 0.0f;
	KeyFrame[0].Ytor = 0.0f;
	KeyFrame[0].Ztor = 0.0f;

	//KF2
	KeyFrame[1].Angt1 = 0.0f;
	KeyFrame[1].Angt2 = 0.0f;
	KeyFrame[1].Angc1 = -5.0f;
	KeyFrame[1].Angc2 = 0.0f;
	KeyFrame[1].Angbi1 = 0.0f;
	KeyFrame[1].Angbi2 = 0.0f;
	KeyFrame[1].Angbib = 0.0f;
	KeyFrame[1].Angbd1 = 0.0f;
	KeyFrame[1].Angbd2 = 0.0f;
	KeyFrame[1].Angbdb = 0.0f;
	KeyFrame[1].Angpizq = 0.0f;
	KeyFrame[1].Angpder = 0.0f;
	KeyFrame[1].Angpi = 0.0f;
	KeyFrame[1].Angpd = 0.0f;
	KeyFrame[1].Xtor = 0.0f;
	KeyFrame[1].Ytor = 0.0f;
	KeyFrame[1].Ztor = 0.0f;

	//KF3
	KeyFrame[2].Angt1 = 0.0f;
	KeyFrame[2].Angt2 = 0.0f;
	KeyFrame[2].Angc1 = 15.0f;
	KeyFrame[2].Angc2 = 0.0f;
	KeyFrame[2].Angbi1 = 0.0f;
	KeyFrame[2].Angbi2 = 0.0f;
	KeyFrame[2].Angbib = 0.0f;
	KeyFrame[2].Angbd1 = 0.0f;
	KeyFrame[2].Angbd2 = 0.0f;
	KeyFrame[2].Angbdb = 0.0f;
	KeyFrame[2].Angpizq = 0.0f;
	KeyFrame[2].Angpder = 0.0f;
	KeyFrame[2].Angpi = 0.0f;
	KeyFrame[2].Angpd = 0.0f;
	KeyFrame[2].Xtor = 0.0f;
	KeyFrame[2].Ytor = 0.0f;
	KeyFrame[2].Ztor = 0.0f;

	//KF4
	KeyFrame[3].Angt1 = -10.0f;
	KeyFrame[3].Angt2 = 0.0f;
	KeyFrame[3].Angc1 = -5.0f;
	KeyFrame[3].Angc2 = 0.0f;
	KeyFrame[3].Angbi1 = 0.0f;
	KeyFrame[3].Angbi2 = 0.0f;
	KeyFrame[3].Angbib = 0.0f;
	KeyFrame[3].Angbd1 = -200.0f;
	KeyFrame[3].Angbd2 = 0.0f;
	KeyFrame[3].Angbdb = 0.0f;
	KeyFrame[3].Angpizq = 10.0f;
	KeyFrame[3].Angpder = 10.0f;
	KeyFrame[3].Angpi = 0.0f;
	KeyFrame[3].Angpd = 0.0f;
	KeyFrame[3].Xtor = 0.0f;
	KeyFrame[3].Ytor = 0.0f;
	KeyFrame[3].Ztor = 0.0f;


	//KF5
	KeyFrame[4].Angt1 = 30.0f;
	KeyFrame[4].Angt2 = 0.0f;
	KeyFrame[4].Angc1 = 0.0f;
	KeyFrame[4].Angc2 = 0.0f;
	KeyFrame[4].Angbi1 = 0.0f;
	KeyFrame[4].Angbi2 = 0.0f;
	KeyFrame[4].Angbib = 0.0f;
	KeyFrame[4].Angbd1 = 50.0f;
	KeyFrame[4].Angbd2 = 0.0f;
	KeyFrame[4].Angbdb = 0.0f;
	KeyFrame[4].Angpizq = -30.0f;
	KeyFrame[4].Angpder = -30.0f;
	KeyFrame[4].Angpi = 0.0f;
	KeyFrame[4].Angpd = 0.0f;
	KeyFrame[4].Xtor = 0.0f;
	KeyFrame[4].Ytor = 0.0f;
	KeyFrame[4].Ztor = 0.0f;

	//KF6
	KeyFrame[5].Angt1 = 30.0f;
	KeyFrame[5].Angt2 = 0.0f;
	KeyFrame[5].Angc1 = 0.0f;
	KeyFrame[5].Angc2 = 0.0f;
	KeyFrame[5].Angbi1 = 0.0f;
	KeyFrame[5].Angbi2 = 0.0f;
	KeyFrame[5].Angbib = 0.0f;
	KeyFrame[5].Angbd1 = 0.0f;
	KeyFrame[5].Angbd2 = 0.0f;
	KeyFrame[5].Angbdb = 0.0f;
	KeyFrame[5].Angpizq = -30.0f;
	KeyFrame[5].Angpder = -30.0f;
	KeyFrame[5].Angpi = 0.0f;
	KeyFrame[5].Angpd = 0.0f;
	KeyFrame[5].Xtor = 0.0f;
	KeyFrame[5].Ytor = 0.0f;
	KeyFrame[5].Ztor = 0.0f;

	//KF7
	KeyFrame[6].Angt1 = 60.0f;
	KeyFrame[6].Angt2 = 0.0f;
	KeyFrame[6].Angc1 = 0.0f;
	KeyFrame[6].Angc2 = 0.0f;
	KeyFrame[6].Angbi1 = 0.0f;
	KeyFrame[6].Angbi2 = 0.0f;
	KeyFrame[6].Angbib = .0f;
	KeyFrame[6].Angbd1 = 0.0f;
	KeyFrame[6].Angbd2 = 0.0f;
	KeyFrame[6].Angbdb = 0.0f;
	KeyFrame[6].Angpizq = 0.0f;
	KeyFrame[6].Angpder = 60.0f;
	KeyFrame[6].Angpi = 0.0f;
	KeyFrame[6].Angpd = 0.0f;
	KeyFrame[6].Xtor = 0.0f;
	KeyFrame[6].Ytor = 10.0f;
	KeyFrame[6].Ztor = 0.0f;

	//KF8
	KeyFrame[7].Angt1 = 360.0f;
	KeyFrame[7].Angt2 = 0.0f;
	KeyFrame[7].Angc1 = 0.0f;
	KeyFrame[7].Angc2 = 0.0f;
	KeyFrame[7].Angbi1 = 0.0f;
	KeyFrame[7].Angbi2 = 0.0f;
	KeyFrame[7].Angbib = .0f;
	KeyFrame[7].Angbd1 = 0.0f;
	KeyFrame[7].Angbd2 = 0.0f;
	KeyFrame[7].Angbdb = 0.0f;
	KeyFrame[7].Angpizq = 0.0f;
	KeyFrame[7].Angpder = 60.0f;
	KeyFrame[7].Angpi = 0.0f;
	KeyFrame[7].Angpd = 0.0f;
	KeyFrame[7].Xtor = 0.0f;
	KeyFrame[7].Ytor = 10.0f;
	KeyFrame[7].Ztor = 0.0f;

	//KF9
	KeyFrame[8].Angt1 = 360.0f;
	KeyFrame[8].Angt2 = 0.0f;
	KeyFrame[8].Angc1 = 0.0f;
	KeyFrame[8].Angc2 = 0.0f;
	KeyFrame[8].Angbi1 = -180.0f;
	KeyFrame[8].Angbi2 = 0.0f;
	KeyFrame[8].Angbib = 0.0f;
	KeyFrame[8].Angbd1 = 0.0f;
	KeyFrame[8].Angbd2 = 0.0f;
	KeyFrame[8].Angbdb = 0.0f;
	KeyFrame[8].Angpizq = 0.0f;
	KeyFrame[8].Angpder = 0.0f;
	KeyFrame[8].Angpi = 0.0f;
	KeyFrame[8].Angpd = 0.0f;
	KeyFrame[8].Xtor = 0.0f;
	KeyFrame[8].Ytor = 0.0f;
	KeyFrame[8].Ztor = 0.0f;
}

int IniGL(GLvoid)										// Aqui se configuran los parametros iniciales de OpenGL
{
	glShadeModel(GL_SMOOTH);							// Activa Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Fondo negro, los primeros tres argumentos definen R/G/B el ultimo es transparencia (Alpha)
	// 1 es el valor maximo que puede tomar
	glClearDepth(1.0f);									// Valor para el Depth Buffer
	glEnable(GL_DEPTH_TEST);							// Activa Depth Testing
	glDepthFunc(GL_LEQUAL);								// Tipo de Depth Testing a usar
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Correccion de c�lculos de perspectiva

	glCullFace(GL_BACK);								// Configurado para eliminar caras traseras
	glEnable(GL_CULL_FACE);								// Activa eliminacion de caras ocultas


	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Posicion de la luz0
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);		// Componente ambiente
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);		// Componente difusa
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);		// Componente especular

	glEnable(GL_LIGHT0);								// Activa luz0
	glEnable(GL_LIGHTING);								// Habilita la iluminaci�n

	e=gluNewQuadric();
	
	posicionCamProv = CVector(0.0f, 10.0f, 80.0f);
	objetivoCamProv = CVector(0.0f, 10.0f, 0.0f);

	iniciaCamara();

        CargaModelos();

        Angt1 = 0.0f;
        Angt2 = 0.0f;
        Angc1 = 0.0f;
        Angc2 = 0.0f;
        Angbi1 = 0.0f;
        Angbi2 = 0.0f;
        Angbi3 = 0.0f;
        Angbd1 = 0.0f;
        Angbd2 = 0.0f;
        Angbd3 = 0.0f;
        Angpizq = 0.0f;
        Angpder = 0.0f;
        Angpi = 0.0f;
        Angpd = 0.0f;

        Xtor = 0.0f;
        Ytor = 0.0f;
        Ztor = 0.0f;

        IncDec = 0; //0: Incrementa valores, 1: Decrementa valores
		
		datColadera.estado = 0;
		datColadera.pos = CVector(30.0f, 0.0f, 0.0f);
		datColadera.vel = 0.5f;
		datColadera.desp = 0.0f;
		datColadera.ang = 0.0f;

		CargaTexturas();
	
		InicializaAnim();
		DatosAnimacion();

		
	return TRUE;										
}

void controlEstadosColadera()
{
	if (datColadera.estado == 1) //Subiendo
	{
		datColadera.pos.y = datColadera.pos.y + datColadera.vel;
		datColadera.desp += datColadera.vel;

		datColadera.ang += 10.0f;
		if (datColadera.ang > 360.0f) datColadera.ang -= 360.0f;

		if (datColadera.desp > 30.0f)
			datColadera.estado = 2;
	}
	else if (datColadera.estado == 2) //Cayendo
	{
		datColadera.pos.y = datColadera.pos.y - datColadera.vel;
		datColadera.desp -= datColadera.vel;

		datColadera.ang += 10.0f;
		if (datColadera.ang > 360.0f) datColadera.ang -= 360.0f;

		if (datColadera.desp <= 0.0f)
		{
			datColadera.estado = 3;
			datColadera.pos.y = 0.0f;
			datColadera.ang = 0.0f;
		}
	}
}

void animacion(int frames)
{
	if (play)
	{
		if ((fabs(KeyFrame[playIndex + 1].Angt1 - Angt1))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angt2 - Angt2))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angc1 - Angc1))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angc2 - Angc2))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angbi1 - Angbi1))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angbi2 - Angbi2))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angbd1 - Angbd1))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angbd2 - Angbd2))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angpizq - Angpizq))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angpder - Angpder))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angpi - Angpi))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Angpd - Angpd))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Xtor - Xtor))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Ytor - Ytor))<0.1f &&
			(fabs(KeyFrame[playIndex + 1].Ztor - Ztor))<0.1f)
		{
			playIndex++;
			if (playIndex > maxKF - 2)
			{
				playIndex = 0;
				play = false;
			}
		}
		else
		{
			KeyFrame[playIndex].incAngt1 = (KeyFrame[playIndex + 1].Angt1 - KeyFrame[playIndex].Angt1) / frames;
			KeyFrame[playIndex].incAngt2 = (KeyFrame[playIndex + 1].Angt2 - KeyFrame[playIndex].Angt2) / frames;
			KeyFrame[playIndex].incAngc1 = (KeyFrame[playIndex + 1].Angc1 - KeyFrame[playIndex].Angc1) / frames;
			KeyFrame[playIndex].incAngc2 = (KeyFrame[playIndex + 1].Angc2 - KeyFrame[playIndex].Angc2) / frames;
			KeyFrame[playIndex].incAngbi1 = (KeyFrame[playIndex + 1].Angbi1 - KeyFrame[playIndex].Angbi1) / frames;
			KeyFrame[playIndex].incAngbi2 = (KeyFrame[playIndex + 1].Angbi2 - KeyFrame[playIndex].Angbi2) / frames;
			KeyFrame[playIndex].incAngbd1 = (KeyFrame[playIndex + 1].Angbd1 - KeyFrame[playIndex].Angbd1) / frames;
			KeyFrame[playIndex].incAngbd2 = (KeyFrame[playIndex + 1].Angbd2 - KeyFrame[playIndex].Angbd2) / frames;
			KeyFrame[playIndex].incAngpizq = (KeyFrame[playIndex + 1].Angpizq - KeyFrame[playIndex].Angpizq) / frames;
			KeyFrame[playIndex].incAngpder = (KeyFrame[playIndex + 1].Angpder - KeyFrame[playIndex].Angpder) / frames;
			KeyFrame[playIndex].incAngpi = (KeyFrame[playIndex + 1].Angpi - KeyFrame[playIndex].Angpi) / frames;
			KeyFrame[playIndex].incAngpd = (KeyFrame[playIndex + 1].Angpd - KeyFrame[playIndex].Angpd) / frames;
			KeyFrame[playIndex].incXtor = (KeyFrame[playIndex + 1].Xtor - KeyFrame[playIndex].Xtor) / frames;
			KeyFrame[playIndex].incYtor = (KeyFrame[playIndex + 1].Ytor - KeyFrame[playIndex].Ytor) / frames;
			KeyFrame[playIndex].incZtor = (KeyFrame[playIndex + 1].Ztor - KeyFrame[playIndex].Ztor) / frames;

			Angt1 += KeyFrame[playIndex].incAngt1;
			Angt2 += KeyFrame[playIndex].incAngt2;
			Angc1 += KeyFrame[playIndex].incAngc1;
			Angc2 += KeyFrame[playIndex].incAngc2;
			Angbi1 += KeyFrame[playIndex].incAngbi1;
			Angbi2 += KeyFrame[playIndex].incAngbi2;
			Angbd1 += KeyFrame[playIndex].incAngbd1;
			Angbd2 += KeyFrame[playIndex].incAngbd2;
			Angpizq += KeyFrame[playIndex].incAngpizq;
			Angpder += KeyFrame[playIndex].incAngpder;
			Angpi += KeyFrame[playIndex].incAngpi;
			Angpd += KeyFrame[playIndex].incAngpd;
			Xtor += KeyFrame[playIndex].incXtor;
			Ytor += KeyFrame[playIndex].incYtor;
			Ztor += KeyFrame[playIndex].incZtor;

		}
	}
}
void SeleccionaMaterial(int tipo)
{
    if (tipo == 0)
    {
        //Material default de OpenGL
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gDefaultDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, gDefaultAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, gDefaultSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, gDefaultShininess);
    }
    else if (tipo == 1)
    {
        //naranja met�lico
        glMaterialfv(GL_FRONT, GL_AMBIENT, nmetalAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, nmetalDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, nmetalSpe);
        glMaterialf(GL_FRONT, GL_SHININESS, nmetalShi);
    }
    else if (tipo == 2)
    {
        //verde met�lico
        glMaterialfv(GL_FRONT, GL_AMBIENT, vmetalAmb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, vmetalDif);
        glMaterialfv(GL_FRONT, GL_SPECULAR, vmetalSpe);
        glMaterialf(GL_FRONT, GL_SHININESS, vmetalShi);
    }
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
		else if (funcion == 2) //Avanza hacia atr�s (z+)
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
            //El radio aumenta y disminuye hasta una distancia fija
		if (funcion == 1) //Disminuye radio de la camara 
		{
                    if (camara2.distCam == 20.0f)
                    {
                        camara2.distCam = camara2.distCam;
                    }
                    else
                    {
                        camara2.distCam -= 0.5f;
                    }
                    camara2.posCam.x = camara2.dirCam.x*camara2.distCam;
                    camara2.posCam.z = camara2.dirCam.z*camara2.distCam;
		}
		else if (funcion == 2) // Aumenta radio de la camara 
		{
                    if (camara2.distCam == 250.0f)
                    {
                        camara2.distCam = camara2.distCam;
                    }
                    else
                    {
                        camara2.distCam += 0.5f;
                    }
                    camara2.posCam.x = camara2.dirCam.x*camara2.distCam;
                    camara2.posCam.z = camara2.dirCam.z*camara2.distCam;
		}

                //Primero: La direccion de la camara va formando un circulo de alrededor del objetivo
                //Por ultimo: La posicion de la camara se va actualizando conforme la ultima posicion que tuvo la direccion y se multiplica por un escalar para formar el radio del circulo
		else if (funcion == 3) //Gira hacia la derecha
		{
                    camara2.angCam -= camara2.velCam;
                    if (camara2.angCam < 360.0f) // Se  para que no haya overflow cuando se deje presionada la tecla
                        camara2.angCam -= 360.0f;

                    camara2.dirCam.x = cos(camara2.angCam*PI / 180.0f);
                    camara2.dirCam.z = sin(camara2.angCam*PI / 180.0f);

                    camara2.posCam.x = camara2.dirCam.x*camara2.distCam;
                    camara2.posCam.z = camara2.dirCam.z*camara2.distCam;
		
		}
		else if (funcion == 4) //Gira hacia la izquierda
		{
                    camara2.angCam += camara2.velCam;
                    if (camara2.angCam > 360.0f) // Se  para que no haya overflow cuando se deje presionada la tecla
                        camara2.angCam -= 360.0f;

                    camara2.dirCam.x = cos(camara2.angCam*PI / 180.0f);
                    camara2.dirCam.z = sin(camara2.angCam*PI / 180.0f);

                    camara2.posCam.x = camara2.dirCam.x*camara2.distCam;
                    camara2.posCam.z = camara2.dirCam.z*camara2.distCam;



		}
		else if (funcion == 5) //Sube pos camara (y+)
		{
			if (camara2.posCam.y == 500) {
				camara2.posCam = camara2.posCam;
			}
			else{
				camara2.posCam.y += camara2.velCam;
			}
			
			camara2.altCam = camara2.posCam.y;
		}
		else if (funcion == 6) //Baja pos camara (y-)
		{
			if (camara2.posCam.y == -500) {
				camara2.posCam = camara2.posCam;
			}
			else {
				camara2.posCam.y -= camara2.velCam;
			}
			camara2.altCam = camara2.posCam.y;
		}
		//C�lculo de la posici�n y objetivo de la c�mara


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
		else if (funcion == 2) //Avanza hacia atr�s
		{
			camara3.posCam = camara3.posCam - camara3.dirCam*camara3.velCam;
			camara3.posCam.y = camara3.altCam;
			camara3.objCam = camara3.posCam + camara3.dirCam*camara3.distCam;
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

void dibujaEsfera(float radio, int paralelos, int meridianos, int modoRender, int semi)
{
    float ang1, ang2;
    float a[3], b[3], c[3], d[3];
    float delta1, delta2, deltaT;

    delta1 = 180.0f / paralelos;
    delta2 = 360.0f / meridianos;
	deltaT = 1.0 / paralelos;
	float t1, t2, s1, s2;
	glEnable(GL_TEXTURE_2D);

	//Semiesfera superior (y positivos)
    for (int i = 0; i < paralelos/2; i++)
    {
		t1 = 0.15+i*deltaT;
		t2 = 0.15+(i + 1.0)*deltaT;
		s1 = 0.0;
		s2 = 1.0;

		for (int j = 0; j <= meridianos; j++)
		{
			ang1 = i*delta1;
			ang2 = j*delta2;
			a[0] = radio*(float)cos(ang1*PI / 180.0f)*(float)cos(ang2*PI / 180.0f);
			a[1] = radio*(float)sin(ang1*PI / 180.0f);
			a[2] = radio*(float)cos(ang1*PI / 180.0f)*(float)sin(ang2*PI / 180.0f);

			ang1 = (i + 1)*delta1;
			ang2 = j*delta2;
			b[0] = radio*(float)cos(ang1*PI / 180.0f)*(float)cos(ang2*PI / 180.0f);
			b[1] = radio*(float)sin(ang1*PI / 180.0f);
			b[2] = radio*(float)cos(ang1*PI / 180.0f)*(float)sin(ang2*PI / 180.0f);
			ang1 = (i + 1)*delta1;
			ang2 = (j + 1)*delta2;
			c[0] = radio*(float)cos(ang1*PI / 180.0f)*(float)cos(ang2*PI / 180.0f);
			c[1] = radio*(float)sin(ang1*PI / 180.0f);
			c[2] = radio*(float)cos(ang1*PI / 180.0f)*(float)sin(ang2*PI / 180.0f);
			ang1 = i*delta1;
			ang2 = (j + 1)*delta2;
			d[0] = radio*(float)cos(ang1*PI / 180.0f)*(float)cos(ang2*PI / 180.0f);
			d[1] = radio*(float)sin(ang1*PI / 180.0f);
			d[2] = radio*(float)cos(ang1*PI / 180.0f)*(float)sin(ang2*PI / 180.0f);

			glColor3f(1.0f, 1.0f, 1.0f);
		

			//Parte superior		
			if (modoRender == 1) glBegin(GL_QUADS);// s�lido
			else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
		

				glBindTexture(GL_TEXTURE_2D, textura[20].texID);
				glBegin(GL_QUADS);			
				glNormal3f(a[0], a[1], a[2]);
				glTexCoord2f(s1*2, t1);
				glVertex3f(a[0], a[1], a[2]);

				glNormal3f(b[0], b[1], b[2]);
				glTexCoord2f(s1*2,t2);
				glVertex3f(b[0], b[1], b[2]);

				glNormal3f(c[0], c[1], c[2]);
				glTexCoord2f(s2*2,t2);
				glVertex3f(c[0], c[1], c[2]);

				glNormal3f(d[0], d[1], d[2]);
				glTexCoord2f(s2*2, t1);
				glVertex3f(d[0], d[1], d[2]);
				
				glEnd();

            //Si la opcion es igual a 0 se dibuja la esfera completa, si es 1, se dibuja solo la mitad
            if (semi == 0)
            {
                //Parte inferior
                if (modoRender == 1) glBegin(GL_QUADS);// s�lido
                else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
                
				glNormal3f(a[0], -a[1], a[2]);
                glVertex3f(a[0], -a[1], a[2]);

                glNormal3f(d[0], -d[1], d[2]);
                glVertex3f(d[0], -d[1], d[2]);

                glNormal3f(c[0], -c[1], c[2]);
                glVertex3f(c[0], -c[1], c[2]);
                
                glNormal3f(b[0], -b[1], b[2]);
                glVertex3f(b[0], -b[1], b[2]);
                glEnd();
            }

        }
    }
	glDisable(GL_TEXTURE_2D);
}


void dibujaEscenario()
{

	int codigo;
	if(modoRender == 1) // Solido (codigo)
		codigo = 0x0007;
	if(modoRender == 2) // Alambrado (GL_LINE_LOOP)
		codigo = 0x0002;

	glEnable(GL_TEXTURE_2D);

	//objeto 1
	glColor3f(1.0f, 0.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textura[0].texID); //Para indicar que imagen de textura se la aplica.

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-120.0f, 0.0f, 17.0f);
	glTexCoord2f(12.0f, 0.0f); glVertex3f(80.0f, 0.0f, 17.0f);
	glTexCoord2f(12.0f, 1.0f); glVertex3f(80.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-120.0f, 0.0f, 0.0f);
	glEnd();

	//objeto 2
	glColor3f(1.0f, 0.4f, 0.0f);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.9f); glVertex3f(-120.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.9f); glVertex3f(50.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, 0.0f, -20.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-120.0f, 0.0f, -20.0f);
	glEnd();

	//objeto 2a
	glColor3f(1.0f, 0.4f, 0.0f);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.9f); glVertex3f(50.0f, 0.0f, -12.0f);
	glTexCoord2f(1.0f, 0.9f); glVertex3f(80.0f, 0.0f, -12.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(80.0f, 0.0f, -30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(50.0f, 0.0f, -30.0f);
	glEnd();


	//objeto 3
	glColor3f(1.0f, 0.8f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textura[1].texID);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-115.0f, 0.0f, -20.0f);
	glTexCoord2f(8.0f, 0.0f); glVertex3f(-50.0f, 0.0f, -20.0f);
	glTexCoord2f(8.0f, 1.0f); glVertex3f(-50.0f, 0.0f, -40.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-115.0f, 0.0f, -40.0f);
	glEnd();

	//objeto 4
	glColor3f(1.0f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textura[14].texID);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(80.0f, 0.0f, 17.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(140.0f, 0.0f, 17.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(140.0f, 0.0f, -30.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(80.0f, 0.0f, -30.0f);
	glEnd();

	//objeto 5
	glColor3f(1.0f, 1.0f, 0.2f);

	glBindTexture(GL_TEXTURE_2D, textura[3].texID);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-110.0f, 0.0f, -40.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-85.0f, 0.0f, -40.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-85.0f, 17.0f, -40.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-110.0f, 17.0f, -40.0f);
	glEnd();

	//objeto 6
	glColor3f(1.0f, 1.0f, 0.6f);

	glBindTexture(GL_TEXTURE_2D, textura[2].texID);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-85.0f, 0.0f, -40.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-60.0f, 0.0f, -40.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-60.0f, 19.0f, -40.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-85.0f, 19.0f, -40.0f);
	glEnd();

	//objeto 7 (2 planos)
	glColor3f(0.0f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textura[4].texID);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-120.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-95.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-95.0f, 22.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-120.0f, 22.0f, 0.0f);
	glEnd();

	glColor3f(0.7f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textura[5].texID);

	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-95.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-95.0f, 0.0f, -18.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-95.0f, 22.0f, -18.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-95.0f, 22.0f, 0.0f);
	glEnd();

	//objeto 8 (10 planos)
	//1
	
	glColor3f(0.0f, 1.0f, 0.4f);

	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f,0.0f );   glVertex3f(-78.0f, 0.0f, 0.0f);
		glTexCoord2f(0.136f, 0.0f); glVertex3f(-68.0f,  0.0f, 0.0f);
		glTexCoord2f(0.136f, 1.0f); glVertex3f(-68.0f, 22.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f);   glVertex3f(-78.0f, 22.0f, 0.0f);
	glEnd();

	//2
	glColor3f(0.0f, 1.0f, 0.8f);

	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.137f, 0.3191f); glVertex3f(-68.0f,  7.0f, 0.0f);
		glTexCoord2f(0.181f, 0.3191f); glVertex3f(-63.0f,  7.0f, 0.0f);
		glTexCoord2f(0.181f, 1.0f); glVertex3f(-63.0f, 22.0f, 0.0f);
		glTexCoord2f(0.137f, 1.0f); glVertex3f(-68.0f, 22.0f, 0.0f);
	glEnd();

	//3
	glColor3f(1.0f, 1.0f, 0.5f);

	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.137f, 0.0f); glVertex3f(-68.0f, 0.0f, 0.0f);
		glTexCoord2f(0.181f, 0.0f); glVertex3f(-63.0f, 0.0f, 0.0f);
		glTexCoord2f(0.181f, 0.046f); glVertex3f(-63.0f, 1.0f, 0.0f);
		glTexCoord2f(0.137f, 0.046f); glVertex3f(-68.0f, 1.0f, 0.0f);
	glEnd();

	//4
	glColor3f(0.5f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.19f, 0.0f); glVertex3f(-63.0f,  0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f,  0.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 22.0f, 0.0f);
		glTexCoord2f(0.19f, 1.0f); glVertex3f(-63.0f, 22.0f, 0.0f);
	glEnd();

	//5
	glColor3f(0.5f, 0.4f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, textura[12].texID);
	glBegin(GL_QUADS);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f,  0.0f,  0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-10.0f,  0.0f,-18.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-10.0f, 22.0f,-18.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 22.0f,  0.0f);
	glEnd();

	//6
	glColor3f(0.5f, 0.8f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, textura[6].texID);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-78.0f,  0.0f,-18.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-78.0f,  0.0f,  0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-78.0f, 22.0f,  0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-78.0f, 22.0f,-18.0f);
	glEnd();

	//7
	glColor3f(1.0f, 0.2f, 0.5f);

	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.137f, 0.046f); glVertex3f(-68.0f, 1.0f, 0.0f);
		glTexCoord2f(0.181f, 0.046f); glVertex3f(-63.0f, 1.0f, 0.0f);
		glTexCoord2f(0.181f, 0.055f); glVertex3f(-63.0f, 1.0f,-5.0f);
		glTexCoord2f(0.137f, 0.055f); glVertex3f(-68.0f, 1.0f,-5.0f);
	glEnd();

	//8
	glColor3f(0.0f, 0.0f, 1.0f);
	
	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.137f, 0.06f); glVertex3f(-68.0f, 1.0f, 0.0f);
		glTexCoord2f(0.181f, 0.06f); glVertex3f(-68.0f, 1.0f,-5.0f);
		glTexCoord2f(0.181f, 0.07f); glVertex3f(-68.0f, 7.0f,-5.0f);
		glTexCoord2f(0.137f, 0.07f); glVertex3f(-68.0f, 7.0f, 0.0f);
	glEnd();

	//9
	glColor3f(0.0f, 0.2f, 1.0f);
	
	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.137f, 0.06f); glVertex3f(-63.0f, 1.0f,-5.0f);
		glTexCoord2f(0.181f, 0.06f); glVertex3f(-63.0f, 1.0f, 0.0f);
		glTexCoord2f(0.181f, 0.07f); glVertex3f(-63.0f, 7.0f, 0.0f);
		glTexCoord2f(0.137f, 0.07f); glVertex3f(-63.0f, 7.0f,-5.0f);
	glEnd();

	//9a

	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.147f, 0.06f); glVertex3f(-68.0f, 1.0f,-5.0f);
		glTexCoord2f(0.181f, 0.06f); glVertex3f(-63.0f, 1.0f,-5.0f);
		glTexCoord2f(0.181f, 0.07f); glVertex3f(-63.0f, 7.0f,-5.0f);
		glTexCoord2f(0.147f, 0.07f); glVertex3f(-68.0f, 7.0f,-5.0f);
	glEnd();
	//Aqu� empiezan los planos creados 


	//11 Shadow
	glColor3f(0.08f, 0.32f, 0.67f);

	glBindTexture(GL_TEXTURE_2D, textura[12].texID);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(50.0f, 0.0f,  0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(50.0f, 22.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(50.0f, 22.0f, -12.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(50.0f, 0.0f, -12.0f);
	glEnd();
	
	//11 Front
	glColor3f(0.5f, 0.73f, 0.95f);

	glBindTexture(GL_TEXTURE_2D, textura[11].texID);
	glBegin(GL_QUADS);		
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(50.0f, 0.0f, 0.0f);
		glTexCoord2f(0.24f, 0.0f); glVertex3f(56.0f, 0.0f, 0.0f);
		glTexCoord2f(0.24f, 1.0f); glVertex3f(56.0f, 22.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(50.0f, 22.0f, 0.0f);
	glEnd();

        //11_inside
		glColor3f(0.0f, 0.29f, 0.29f);
        
		glBindTexture(GL_TEXTURE_2D, textura[11].texID);
		glBegin(GL_QUADS);
        		glNormal3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.26f, 0.2f); glVertex3f(56.0f, 1.0f, 0.0f);
				glTexCoord2f(0.35f, 0.2f); glVertex3f(56.0f, 1.0f, -5.0f);
                glTexCoord2f(0.35f, 0.3f); glVertex3f(56.0f, 7.0f, -5.0f);
				glTexCoord2f(0.26f, 0.3f); glVertex3f(56.0f, 7.0f, 0.0f);
        glEnd();

	//12 Front_a
	glColor3f(0.92f, 0.80f, 0.91f);

	glBindTexture(GL_TEXTURE_2D, textura[11].texID);
	glBegin(GL_QUADS);		
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.26f, 0.0f); glVertex3f(56.0f, 0.0f,  0.0f);
		glTexCoord2f(0.35f, 0.0f); glVertex3f(62.0f, 0.0f, 0.0f);
		glTexCoord2f(0.35f, 0.055f); glVertex3f(62.0f, 1.0f, 0.0f);
		glTexCoord2f(0.26f, 0.055f); glVertex3f(56.0f, 1.0f, 0.0f);
	glEnd();

        //12 Front_a_Up_Cover
		glColor3f(0.66f, 0.0f, 0.66f);
		glBindTexture(GL_TEXTURE_2D, textura[7].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(0.137f, 0.046f); glVertex3f(56.0f, 1.0f, 0.0f);
				glTexCoord2f(0.181f, 0.046f); glVertex3f(62.0f, 1.0f, 0.0f);
				glTexCoord2f(0.181f, 0.055f); glVertex3f(62.0f, 1.0f, -5.0f);
				glTexCoord2f(0.137f, 0.055f); glVertex3f(56.0f, 1.0f, -5.0f);
 
        glEnd();
	
	//12 Front_b
	glColor3f(0.54f, 0.18f, 0.98f);

	glBindTexture(GL_TEXTURE_2D, textura[11].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.25f, 0.319f); glVertex3f(56.0f, 7.0f,  0.0f);
		glTexCoord2f(0.361f, 0.319f); glVertex3f(62.0f, 7.0f, 0.0f);
		glTexCoord2f(0.361f, 1.0f); glVertex3f(62.0f, 22.0f, 0.0f);
		glTexCoord2f(0.25f, 1.0f); glVertex3f(56.0f, 22.0f, 0.0f);
	glEnd();

	//13 Front
	glColor3f(0.95f, 0.4f, 0.95f);

	glBindTexture(GL_TEXTURE_2D, textura[11].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.37f, 0.0f); glVertex3f(62.0f, 0.0f,  0.0f);
		glTexCoord2f(0.63f, 0.0f); glVertex3f(68.0f, 0.0f, 0.0f);
		glTexCoord2f(0.63f, 1.0f); glVertex3f(68.0f, 22.0f, 0.0f);
		glTexCoord2f(0.37f, 1.0f); glVertex3f(62.f, 22.0f, 0.0f);
	glEnd();

        //13_inside_a

        glColor3f(0.4f, 0.0f, 0.4f);
		glBindTexture(GL_TEXTURE_2D, textura[11].texID);
		glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.26f, 0.2f); glVertex3f(62.0f, 1.0f, 0.0f);
		glTexCoord2f(0.35f, 0.2f); glVertex3f(62.0f, 7.0f, 0.0f); 
		glTexCoord2f(0.35f, 0.3f); glVertex3f(62.0f, 7.0f, -5.0f); 
		glTexCoord2f(0.26f, 0.3f); glVertex3f(62.0f, 1.0f, -5.0f); 

        glEnd();

        //13_inside_b
		glColor3f(0.4f, 0.0f, 0.4f);
		glBindTexture(GL_TEXTURE_2D, textura[11].texID);
		glBegin(GL_QUADS);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.26f, 0.2f); glVertex3f(68.0f, 1.0f, -5.0f);
		glTexCoord2f(0.35f, 0.2f); glVertex3f(68.0f, 7.0f, -5.0f);
		glTexCoord2f(0.35f, 0.3f); glVertex3f(68.0f, 7.0f, 0.0f);
		glTexCoord2f(0.26f, 0.3f); glVertex3f(68.0f, 1.0f, 0.0f);   
      
        glEnd();



	//14 Front_a
	glColor3f(0.58f, 0.84f, 0.61f);
	glBindTexture(GL_TEXTURE_2D, textura[11].texID);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.26f, 0.0f); 	glVertex3f(68.0f, 0.0f, 0.0f);
	glTexCoord2f(0.35f, 0.0f); glVertex3f(74.0f, 0.0f, 0.0f);
	glTexCoord2f(0.35f, 0.055f); glVertex3f(74.0f, 1.0f, 0.0f);
	glTexCoord2f(0.26f, 0.055f); glVertex3f(68.0f, 1.0f, 0.0f);
	glEnd();

        //14 
	glBindTexture(GL_TEXTURE_2D, textura[7].texID);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.137f, 0.046f); glVertex3f(68.0f, 1.0f, 0.0f);
	glTexCoord2f(0.181f, 0.046f); glVertex3f(74.0f, 1.0f, 0.0f);
	glTexCoord2f(0.181f, 0.055f); glVertex3f(74.0f, 1.0f, -5.0f);
	glTexCoord2f(0.137f, 0.055f); glVertex3f(68.0f, 1.0f, -5.0f);
    glEnd();
	
	//14 Front_b
	glColor3f(0.19f, 0.55f, 0.23f);
	glBindTexture(GL_TEXTURE_2D, textura[11].texID);
	glBegin(GL_QUADS);
	 glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.63f, 0.319f); glVertex3f(68.0f, 7.0f,  0.0f);
	glTexCoord2f(0.75f, 0.319f); glVertex3f(74.0f, 7.0f, 0.0f);
	glTexCoord2f(0.75f, 1.0f);   glVertex3f(74.0f, 22.0f, 0.0f);
	glTexCoord2f(0.63f, 1.0f);	 glVertex3f(68.0f, 22.0f, 0.0f);
	glEnd();

        //15 _Front
		glColor3f(1.0f, 0.50f, 0.25f);

		glBindTexture(GL_TEXTURE_2D, textura[11].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.76f, 0.0f); glVertex3f(74.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(80.0f, 0.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(80.0f, 22.0f, 0.0f);
				glTexCoord2f(0.76f, 1.0f); glVertex3f(74.0f, 22.0f, 0.0f);
        glEnd();

        //15_inside
		glColor3f(0.49f, 0.247f, 0.0f);

		glBindTexture(GL_TEXTURE_2D, textura[7].texID);
		glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.137f, 0.06f); glVertex3f(74.0f, 1.0f, 0.0f);
		glTexCoord2f(0.181f, 0.06f); glVertex3f(74.0f, 7.0f, 0.0f);
		glTexCoord2f(0.181f, 0.07f); glVertex3f(74.0f, 7.0f, -5.0f);
		glTexCoord2f(0.137f, 0.07f); glVertex3f(74.0f, 1.0f, -5.0f);
        glEnd();


		
        //15_Shadow
		glColor3f(0.784f, 0.411f, 0.015f);

		glBindTexture(GL_TEXTURE_2D, textura[7].texID);
		glBegin(GL_QUADS);
				glNormal3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.137f, 0.06f); glVertex3f(80.0f, 0.0f, 0.0f);
				glTexCoord2f(0.181f, 0.06f); glVertex3f(80.0f, 0.0f, -12.0f);
				glTexCoord2f(0.181f, 0.07f); glVertex3f(80.0f, 22.0f, -12.0f);
				glTexCoord2f(0.137f, 0.07f); glVertex3f(80.0f, 22.0f, 0.0f);
        glEnd();

        //15_Box_inside_shadow
		glColor3f(0.45f, 0.45f, 0.45f);
		glBindTexture(GL_TEXTURE_2D, textura[7].texID);
		glBegin(GL_QUADS);
				 glNormal3f(0.0f, 0.0f, 1.0f);
				 glTexCoord2f(0.137f, 0.06f); glVertex3f(56.0f, 1.0f, -5.0f);
				 glTexCoord2f(0.181f, 0.06f); glVertex3f(74.0f, 1.0f, -5.0f);
				 glTexCoord2f(0.181f, 0.07f); glVertex3f(74.0f, 7.0f, -5.0f);
				 glTexCoord2f(0.181f, 0.07f); glVertex3f(56.0f, 7.0f, -5.0f);
        glEnd();

        //16_Utter_wall
		glColor3f(0.4f, 0.4f, 0.0f);
		glBindTexture(GL_TEXTURE_2D, textura[13].texID);
		glBegin(GL_QUADS);
		 glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.5f); glVertex3f(140.0f, 0.0f, 17.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(140.0f, 22.0f, 17.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(140.0f, 22.0f, -30.0f);
		glTexCoord2f(1.0f, 0.5f); glVertex3f(140.0f, 0.0f, -30.0f);
        glEnd();

        //17_Background_Wall
		glColor3f(0.23f, 0.403f, 0.615f);
		glBindTexture(GL_TEXTURE_2D, textura[13].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(68.0f, 0.0f, -30.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(140.0f, 0.0f, -30.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(140.0f, 22.0f, -30.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(68.0f, 22.0f, -30.0f);
        glEnd();

        //20_Back_floor (woof fence)
        glColor3f(1.0f, 0.65f, 0.0f);
		glBindTexture(GL_TEXTURE_2D, textura[9].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 1.0f, 0.0f);

				glTexCoord2f(0.0f, 0.0f); glVertex3f(55.0f, 0.0f, -20.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(55.0f, 9.0f, -20.0f);
				glTexCoord2f(10.0f, 1.0f); glVertex3f(-50.0f, 9.0f, -20.0f);
				glTexCoord2f(10.0f, 0.0f); glVertex3f(-50.0f, 0.0f, -20.0f);
        glEnd();

        //Building1_a
		glColor3f(0.20f, 0.52f, 0.16f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				 glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.14f, 0.0f); glVertex3f(50.0f, 0.0f, -30.0f);
				glTexCoord2f(0.14f,1.0f); glVertex3f(50.0f, 22.0f, -30.0f);
                glTexCoord2f(0.42f,1.0f);glVertex3f(40.0f, 22.0f, -30.0f);
				glTexCoord2f(0.42f,0.0f); glVertex3f(40.0f, 0.0f, -30.0f);
        glEnd();

        //Building1_b
		glColor3f(0.26f, 0.67f, 0.21f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.69f,0.0f); glVertex3f(40.0f, 0.0f, -30.0f);
				glTexCoord2f(0.69f,1.0f); glVertex3f(40.0f, 22.0f, -30.0f);
				glTexCoord2f(0.71f,1.0f); glVertex3f(40.0f, 22.0f, -34.0f);
				glTexCoord2f(0.71f,0.0f); glVertex3f(40.0f, 0.0f, -34.0f);
        glEnd();

        //Building1_c
		glColor3f(0.18f, 0.47f, 0.152f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
			 glNormal3f(1.0f, 0.0f, 0.0f);
			 glTexCoord2f(0.71f,0.0f);glVertex3f(50.0f, 0.0f, -34.0f);
			 glTexCoord2f(0.71f,1.0f);glVertex3f(50.0f, 22.0f, -34.0f);
             glTexCoord2f(0.69f,1.0f); glVertex3f(50.0f, 22.0f, -30.0f);
			 glTexCoord2f(0.69f,0.0f); glVertex3f(50.0f, 0.0f, -30.0f);
        glEnd();

        //Building1_Wall_a
		glColor3f(0.145f, 0.478f, 0.447f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(1.0f,0.0f); glVertex3f(60.0f, 0.0f, -34.0f);
				glTexCoord2f(1.0f,1.0f); glVertex3f(60.0f, 20.0f, -34.0f);
				glTexCoord2f(0.79f,1.0f); glVertex3f(50.0f, 20.0f, -34.0f);
				glTexCoord2f(0.79f,0.0f); glVertex3f(50.0f, 0.0f, -34.0f);
        glEnd();

        //Building1_Wall_b
		glColor3f(0.623f, 0.639f, 0.211f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.13f,0.0f);  glVertex3f(40.0f, 0.0f, -34.0f);
				glTexCoord2f(0.13f,1.0f); glVertex3f(40.0f, 20.0f, -34.0f);
				glTexCoord2f(0.0f,1.0f); glVertex3f(30.0f, 20.0f, -34.0f);
				glTexCoord2f(0.0f,0.0f); glVertex3f(30.0f, 0.0f, -34.0f);
        glEnd();

        //Building1_Wall_c
		glColor3f(0.98f, 0.658f, 0.541f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01f,0.0f); glVertex3f(30.0f, 0.0f, -34.0f);
				glTexCoord2f(0.01f,0.89f); glVertex3f(30.0f, 20.0f, -34.0f);
				glTexCoord2f(0.03f,0.89f); glVertex3f(30.0f, 20.0f, -46.0f);
				glTexCoord2f(0.03f,0.0f); glVertex3f(30.0f, 0.0f, -46.0f);
        glEnd();

        //Building2_Wall_a
		glColor3f(0.98f, 0.658f, 0.541f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01f,0.0f); glVertex3f(20.0f, 0.0f, -46.0f);
				glTexCoord2f(0.01f,0.89f); glVertex3f(20.0f, 20.0f, -46.0f);
				glTexCoord2f(0.03f,0.89f); glVertex3f(20.0f, 20.0f, -34.0f);
				glTexCoord2f(0.03f,0.0f); glVertex3f(20.0f, 0.0f, -34.0f);
        glEnd();

        //Building2_Wall_b
		glColor3f(0.133f, 0.658f, 0.501f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.45,0.0); glVertex3f(20.0f, 0.0f, -34.0f);
				glTexCoord2f(0.45,1.0); glVertex3f(20.0f, 20.0f, -34.0f);
				glTexCoord2f(0.68,1.0); glVertex3f(10.0f, 20.0f, -34.0f);
				glTexCoord2f(0.68,0.0); glVertex3f(10.0f, 0.0f, -34.0f);
        glEnd();

    //Building2_a

	    glColor3f(0.13f, 0.568f, 0.658f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.71,0.0); glVertex3f(10.0f, 0.0f, -30.0f);
				glTexCoord2f(0.71,1.0); glVertex3f(10.0f, 22.0f, -30.0f);
				glTexCoord2f(0.76,1.0); glVertex3f(8.0f, 22.0f, -30.0f);
				glTexCoord2f(0.76,0.0); glVertex3f(8.0f, 0.0f, -30.0f);
        glEnd();

        //Building2_b
		glColor3f(0.26f, 0.67f, 0.21f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01,0.0); glVertex3f(8.0f, 0.0f, -30.0f);
				glTexCoord2f(0.01,0.89); glVertex3f(8.0f, 22.0f, -30.0f);
				glTexCoord2f(0.03,0.89 ); glVertex3f(8.0f, 22.0f, -34.0f);
				glTexCoord2f(0.03,0.0); glVertex3f(8.0f, 0.0f, -34.0f);
        glEnd();

        //Building2_c
		glColor3f(0.18f, 0.47f, 0.152f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01, 0.0); glVertex3f(10.0f, 0.0f, -34.0f);
				glTexCoord2f(0.01,0.89); glVertex3f(10.0f, 22.0f, -34.0f);
				glTexCoord2f(0.03,0.89); glVertex3f(10.0f, 22.0f, -30.0f);
				glTexCoord2f(0.03,0.0); glVertex3f(10.0f, 0.0f, -30.0f);
        glEnd();


        //Building4_Wall_a
		glColor3f(0.145f, 0.478f, 0.447f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.45,0.0); glVertex3f(8.0f, 0.0f, -34.0f);
				glTexCoord2f(0.45,1.0); glVertex3f(8.0f, 20.0f, -34.0f);
				glTexCoord2f(0.68,1.0); glVertex3f(-2.0f, 20.0f, -34.0f);
				glTexCoord2f(0.68,0.0 ); glVertex3f(-2.0f, 0.0f, -34.0f);
        glEnd();

        //Building4_a

		glColor3f(0.13f, 0.568f, 0.658f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.14,0.0); glVertex3f(-2.0f, 0.0f, -30.0f);
				glTexCoord2f(0.14,1.0 ); glVertex3f(-2.0f, 22.0f, -30.0f);
				glTexCoord2f(0.41,1.0); glVertex3f(-12.0f, 22.0f, -30.0f);
				glTexCoord2f(0.41,0.0); glVertex3f(-12.0f, 0.0f, -30.0f);
        glEnd();

        //Building4_b
		glColor3f(0.26f, 0.67f, 0.21f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);              
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01,0.0);  glVertex3f(-12.0f, 0.0f, -30.0f);
				glTexCoord2f(0.01,0.89 ); glVertex3f(-12.0f, 22.0f, -30.0f);
				glTexCoord2f(0.03,0.89); glVertex3f(-12.0f, 22.0f, -34.0f);
				glTexCoord2f(0.03,0.0); glVertex3f(-12.0f, 0.0f, -34.0f);
        glEnd();

        //Building4_c
		glColor3f(0.18f, 0.47f, 0.152f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01,0.0); glVertex3f(-2.0f, 0.0f, -34.0f);
				glTexCoord2f(0.01,0.89); glVertex3f(-2.0f, 22.0f, -34.0f);
				glTexCoord2f(0.03,0.89); glVertex3f(-2.0f, 22.0f, -30.0f);
				glTexCoord2f(0.03, 0.0); glVertex3f(-2.0f, 0.0f, -30.0f);
        glEnd();


        //Building4_Wall_b
		glColor3f(0.623f, 0.639f, 0.211f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
         	glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.44,0.0);   glVertex3f(-12.0f, 0.0f, -34.0f);
			glTexCoord2f(0.44,1.0); glVertex3f(-12.0f, 20.0f, -34.0f);
			glTexCoord2f(0.68,1.0); glVertex3f(-22.0f, 20.0f, -34.0f);
			glTexCoord2f(0.68,0.0); glVertex3f(-22.0f, 0.0f, -34.0f);
        glEnd();

        //Building5_a

		glColor3f(0.13f, 0.568f, 0.658f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.71,0.0 ); glVertex3f(-22.0f, 0.0f, -30.0f);
				glTexCoord2f(0.71,1.0); glVertex3f(-22.0f, 22.0f, -30.0f);
				glTexCoord2f(0.76,1.0); glVertex3f(-24.0f, 22.0f, -30.0f);
				glTexCoord2f(0.76,0.0 ); glVertex3f(-24.0f, 0.0f, -30.0f);
        glEnd();

        //Building5_b
		glColor3f(0.26f, 0.67f, 0.21f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
       
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01, 0.0); glVertex3f(-24.0f, 0.0f, -30.0f);
				glTexCoord2f(0.01, 0.89); glVertex3f(-24.0f, 22.0f, -30.0f);
				glTexCoord2f(0.03, 0.89); glVertex3f(-24.0f, 22.0f, -34.0f);
				glTexCoord2f(0.03, 0.0); glVertex3f(-24.0f, 0.0f, -34.0f);
        glEnd();

        //Building5_c
		glColor3f(0.18f, 0.47f, 0.152f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
            
				glNormal3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01, 0.0); glVertex3f(-22.0f, 0.0f, -34.0f);
				glTexCoord2f(0.01, 0.89); glVertex3f(-22.0f, 22.0f, -34.0f);
				glTexCoord2f(0.03, 0.89); glVertex3f(-22.0f, 22.0f, -30.0f);
				glTexCoord2f(0.03, 0.0); glVertex3f(-22.0f, 0.0f, -30.0f);
        glEnd();


        //Building5_Wall_a
		glColor3f(0.145f, 0.478f, 0.447f);
        glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.45, 0.0); glVertex3f(-24.0f, 0.0f, -34.0f);
				glTexCoord2f(0.45, 1.0); glVertex3f(-24.0f, 20.0f, -34.0f);
				glTexCoord2f(0.68, 1.0); glVertex3f(-34.0f, 20.0f, -34.0f);
				glTexCoord2f(0.68, 0.0); glVertex3f(-34.0f, 0.0f, -34.0f);
        glEnd();


        //Building6_a
       glColor3f(0.13f, 0.568f, 0.658f);
	   glBindTexture(GL_TEXTURE_2D, textura[8].texID);
	   glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.41, 0.0); glVertex3f(-34.0f, 0.0f, -30.0f);
				glTexCoord2f(0.41, 1.0); glVertex3f(-34.0f, 22.0f, -30.0f);
				glTexCoord2f(0.15, 1.0); glVertex3f(-44.0f, 22.0f, -30.0f);
				glTexCoord2f(0.15, 0.0); glVertex3f(-44.0f, 0.0f, -30.0f);
        glEnd();

        //Building6_b
		glColor3f(0.26f, 0.67f, 0.21f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01, 0.0); glVertex3f(-44.0f, 0.0f, -30.0f);
				glTexCoord2f(0.01, 0.89); glVertex3f(-44.0f, 22.0f, -30.0f);
				glTexCoord2f(0.03, 0.89); glVertex3f(-44.0f, 22.0f, -34.0f);
				glTexCoord2f(0.03, 0.0); glVertex3f(-44.0f, 0.0f, -34.0f);
        glEnd();

        //Building6_c
		glColor3f(0.18f, 0.47f, 0.152f);
		glBindTexture(GL_TEXTURE_2D, textura[8].texID);
		glBegin(GL_QUADS);
				glNormal3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.01, 0.0); glVertex3f(-34.0f, 0.0f, -34.0f);
				glTexCoord2f(0.01, 0.89); glVertex3f(-34.0f, 22.0f, -34.0f);
				glTexCoord2f(0.03, 0.89); glVertex3f(-34.0f, 22.0f, -30.0f);
				glTexCoord2f(0.03, 0.0); glVertex3f(-34.0f, 0.0f, -30.0f);
        glEnd();

        //Weird_table_thing_a
	    glColor3f(0.27f, 0.48f, 0.521f);
		glBindTexture(GL_TEXTURE_2D, textura[16].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0, 0.0); glVertex3f(-45.0f, 5.0f, 0.0f);
                glTexCoord2f(1.0, 0.0); glVertex3f(-55.0f, 5.0f, 0.0f);
				glTexCoord2f(1.0, 1.0); glVertex3f(-55.0f, 5.0f, 5.0f);
				glTexCoord2f(0.0, 1.0); glVertex3f(-45.0f, 5.0f, 5.0f);
        glEnd();

        //Weird_table_thing_b
        glColor3f(0.16f, 0.29f, 0.30f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
        
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.0, 1.0); glVertex3f(-55.0f, 5.0f, 0.0f);
				glTexCoord2f(0.0, 0.0); glVertex3f(-55.0f, 2.0f, 0.0f);
				glTexCoord2f(1.0, 0.0); glVertex3f(-55.0f, 2.0f, 5.0f);
				glTexCoord2f(1.0, 1.0); glVertex3f(-55.0f, 5.0f, 5.0f);
        glEnd();

        //Weird_table_thing_c
        glColor3f(0.16f, 0.29f, 0.30f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
				glNormal3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.0, 0.0); glVertex3f(-45.0f, 2.0f, 0.0f);
				glTexCoord2f(0.0, 1.0); glVertex3f(-45.0f, 5.0f, 0.0f);
				glTexCoord2f(1.0, 1.0); glVertex3f(-45.0f, 5.0f, 5.0f);
				glTexCoord2f(1.0, 0.0); glVertex3f(-45.0f, 2.0f, 5.0f);
        glEnd();

        //Weird_table_thing_d
        glColor3f(0.443f, 0.52f, 0.27f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
       
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.0,0.0); glVertex3f(-45.0f, 2.0f, 5.0f);
				glTexCoord2f(0.0,1.0); glVertex3f(-45.0f, 5.0f, 5.0f);
				glTexCoord2f(2.0,1.0); glVertex3f(-55.0f, 5.0f, 5.0f);
				glTexCoord2f(2.0,0.0); glVertex3f(-55.0f, 2.0f, 5.0f);
        glEnd();

        //Weird_table_thing_e
	    glColor3f(0.16f, 0.25f, 0.23f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
		//	glNormal3f(0.0f, 0.0f, 1.0f); // No se como :(
				glTexCoord2f(0.0, 0.85); glVertex3f(-55.0f, 2.0f, 0.0f);
				glTexCoord2f(0.0, 0.79); glVertex3f(-54.0f, 1.0f, 1.0f);
				glTexCoord2f(1.0, 0.79); glVertex3f(-54.0f, 1.0f, 4.0f);
				glTexCoord2f(1.0, 0.85); glVertex3f(-55.0f, 2.0f, 5.0f);
        glEnd();

        //Weird_table_thing_f
		glColor3f(0.50f, 0.32f, 0.458f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
			//	glNormal3f(0.0f, 0.0f, 1.0f); //No se como :(
				glTexCoord2f(1.0, 0.85); glVertex3f(-45.0f, 2.0f, 5.0f);
				glTexCoord2f(0.0, 0.85); glVertex3f(-55.0f, 2.0f, 5.0f);
				glTexCoord2f(0.0, 0.79); glVertex3f(-54.0f, 1.0f, 4.0f);
				glTexCoord2f(1.0, 0.79); glVertex3f(-46.0f, 1.0f, 4.0f);
        glEnd();

        //Weird_table_thing_g
	    glColor3f(0.16f, 0.25f, 0.23f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
          
		//	glNormal3f(0.0f, 0.0f, 1.0f); // No se como :(
				glTexCoord2f(1.0, 0.85); glVertex3f(-45.0f, 2.0f, 0.0f);
				glTexCoord2f(1.0, 0.79); glVertex3f(-45.0f, 2.0f, 5.0f);
				glTexCoord2f(0.0, 0.79); glVertex3f(-46.0f, 1.0f, 4.0f);
				glTexCoord2f(0.0, 0.85); glVertex3f(-46.0f, 1.0f, 1.0f);
        glEnd();

        //Weird_table_thing_h
	    glColor3f(0.5f, 1.0f, 0.623f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
            
				glNormal3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.0, 0.86); glVertex3f(-46.0f, 0.0f, 1.0f);
				glTexCoord2f(0.0, 1.0); glVertex3f(-46.0f, 1.0f, 1.0f);
				glTexCoord2f(1.0, 1.0); glVertex3f(-46.0f, 1.0f, 4.0f);
				glTexCoord2f(1.0, 0.86); glVertex3f(-46.0f, 0.0f, 4.0f);
        glEnd();

        //Weird_table_thing_i
        glColor3f(0.694f, 0.176f, 0.80f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(1.0, 0.86); glVertex3f(-46.0f, 0.0f, 4.0f);
				glTexCoord2f(1.0, 1.0); glVertex3f(-46.0f, 1.0f, 4.0f);
				glTexCoord2f(0.0, 1.0); glVertex3f(-54.0f, 1.0f, 4.0f);
				glTexCoord2f(0.0, 0.86); glVertex3f(-54.0f, 0.0f, 4.0f);
        glEnd();

        //Weird_table_thing_j
		glColor3f(0.5f, 1.0f, 0.623f);
		glBindTexture(GL_TEXTURE_2D, textura[15].texID);
		glBegin(GL_QUADS);
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.0, 0.86); glVertex3f(-54.0f, 0.0f, 4.0f);
				glTexCoord2f(0.0, 1.0); glVertex3f(-54.0f, 1.0f, 4.0f);
				glTexCoord2f(1.0, 1.0); glVertex3f(-54.0f, 1.0f, 1.0f);
				glTexCoord2f(1.0, 0.86); glVertex3f(-54.0f, 0.0f, 1.0f);
        glEnd();

		//Se paso aqui porque tiene transparencia, si primero lo ponemos lo dem�s ya no se ve. 
		glColor3f(0.11f, 0.97f, 0.88f);
		glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
			glBindTexture(GL_TEXTURE_2D, textura[10].texID);
			glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, 0.0f, -3.0f);
				glTexCoord2f(4.0f, 0.0f); glVertex3f(50.0f, 0.0f, -3.0f);
				glTexCoord2f(4.0f, 1.0f); glVertex3f(50.0f, 8.0f, -3.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-10.0f, 8.0f, -3.0f);
			glEnd();
		glDisable(GL_ALPHA_TEST);


		//18_Fence
		glColor3f(0.20f, 0.278f, 0.286f);
		glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
			glBindTexture(GL_TEXTURE_2D, textura[10].texID);
			glBegin(GL_QUADS);
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(130.0f, 0.0f, 17.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(130.0f, 9.0f, 17.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(130.0f, 9.0f, -26.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(130.0f, 0.0f, -26.0f);
			glEnd();
		glDisable(GL_ALPHA_TEST);

		//19_Background_Fence_a
		glColor3f(0.4f, 0.4f, 0.0f);
		glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
			glBindTexture(GL_TEXTURE_2D, textura[10].texID);
			glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(130.0f, 0.0f, -26.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(130.0f, 9.0f, -26.0f);
				glTexCoord2f(2.0f, 1.0f); glVertex3f(105.0f, 9.0f, -26.0f);
				glTexCoord2f(2.0f, 0.0f); glVertex3f(105.0f, 0.0f, -26.0f);
			glEnd();
		glDisable(GL_ALPHA_TEST);

		//19_Background_Fence_b
		glColor3f(0.4f, 0.4f, 0.0f);
		glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
			glBindTexture(GL_TEXTURE_2D, textura[10].texID);
			glBegin(GL_QUADS);
				glNormal3f(0.0f, 0.0f, 1.0f);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(90.0f, 0.0f, -26.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(90.0f, 9.0f, -26.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(68.0f, 9.0f, -26.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(68.0f, 0.0f, -26.0f);
			glEnd();
		glDisable(GL_ALPHA_TEST);
		
	glDisable(GL_TEXTURE_2D);

	//Al final de la funci�n siempre se regresa al color blanco default de opengl
	glColor3f(1.0f,1.0f,1.0f);

}

void boxWallA(float x, float y, float z, int modoRender)
{
    glColor3f(1.0f, 1.0f, 0.0f);
    if (modoRender == 1) glBegin(GL_QUADS);// s�lido
    else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, y, 0.0f);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x, 0.0f, 0.0f);
    glEnd();
}

void boxWallB(float x, float y, float z, int modoRender)
{
    glColor3f(-1.0f, 0.0f, 0.0f);

    if (modoRender == 1) glBegin(GL_QUADS);// s�lido
    else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, z);
    glVertex3f(0.0f, y, z);
    glVertex3f(0.0f, y, 0.0f);
    glEnd();
}

void boxWallC(float x, float y, float z, int modoRender)
{
    glColor3f(0.0f, 0.0f, 1.0f);
    if (modoRender == 1) glBegin(GL_QUADS);// s�lido
    else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(x, 0.0f, 0.0f);
    glVertex3f(x, 0.0f, z);
    glVertex3f(0.0f, 0.0f, z);
    glEnd();
}


void dibujaCaja(float ancho, float altura, float largo, int modoRender)
{
   //Cara A
    boxWallA(ancho, altura, largo, modoRender);
   //Traslacion para Cara A Here:
    glPushMatrix();
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-ancho, 0.0f, -largo);
        boxWallA(ancho, altura, largo, modoRender);
    glPopMatrix();
   //Cara B
    boxWallB(ancho, altura, largo, modoRender);

    glPushMatrix();
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(-ancho, 0.0f, -largo);

        boxWallB(ancho, altura, largo, modoRender);
    glPopMatrix();
   //Cara C
    boxWallC(ancho, altura, largo, modoRender);
    glPushMatrix();
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, -altura, -largo);
        boxWallC(ancho, altura, largo, modoRender);
    glPopMatrix();


   glColor3f(1.0f, 1.0f, 1.0f);

    
}

void dibujaCilindro(float radio, int lados, float altura, int modoRender,int textu) {
	float ang;
	float a[3], b[3], c[3], d[3];
	float delta;

	CVector normalVert[4];

	delta = 360.0f / lados;

	for (int i = 0; i < lados; i++)
	{
		ang = i*delta;

		a[0] = radio*(float)cos(ang*PI / 180.0f);
		a[1] = 0.0f;
		a[2] = radio*(float)sin(ang*PI / 180.0f);

		b[0] = a[0];
		b[1] = altura;
		b[2] = a[2];

		ang = (i + 1)*delta;

		c[0] = radio*(float)cos(ang*PI / 180.0f);
		c[1] = altura;
		c[2] = radio*(float)sin(ang*PI / 180.0f);

		d[0] = c[0];
		d[1] = 0.0f;
		d[2] = c[2];

		glColor3f(1.0f, 0.0f, 0.0f);

		//C�lculo de las normales
		normalVert[0] = Normaliza(CVector(a[0], a[1], a[2]) - CVector(0.0f, 0.0f, 0.0f));
		normalVert[1] = Normaliza(CVector(b[0], b[1], b[2]) - CVector(0.0f, altura, 0.0f));
		normalVert[2] = Normaliza(CVector(c[0], c[1], c[2]) - CVector(0.0f, altura, 0.0f));
		normalVert[3] = Normaliza(CVector(d[0], d[1], d[2]) - CVector(0.0f, 0.0f, 0.0f));

		if (modoRender == 1) glBegin(GL_QUADS);// s�lido
		else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
		glNormal3f(normalVert[0].x, normalVert[0].y, normalVert[0].z);
		glVertex3f(a[0], a[1], a[2]);
		glNormal3f(normalVert[1].x, normalVert[1].y, normalVert[1].z);
		glVertex3f(b[0], b[1], b[2]);
		glNormal3f(normalVert[2].x, normalVert[2].y, normalVert[2].z);
		glVertex3f(c[0], c[1], c[2]);
		glNormal3f(normalVert[3].x, normalVert[3].y, normalVert[3].z);
		glVertex3f(d[0], d[1], d[2]);
		glEnd();

		//Tapa superior
		glColor3f(1.0f, 1.0f, 0.0f);

		if (modoRender == 1) glBegin(GL_TRIANGLES);// s�lido
		else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(c[0], c[1], c[2]);
		glVertex3f(b[0], b[1], b[2]);
		glVertex3f(0.0f, altura, 0.0f);
		glEnd();

		//Tapa inferior
		glColor3f(0.0f, 0.0f, 1.0f);

		if (modoRender == 1) glBegin(GL_TRIANGLES);// s�lido
		else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(a[0], a[1], a[2]);
		glVertex3f(d[0], d[1], d[2]);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		//Texturizado
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textura[20].texID);
		glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0);
		if (textu == 1) {
			glTexCoord2f(1.0f, 0.0f);
		}
		else if (textu == 2)
		{
			glTexCoord2f(1.0f, 0.0f);
		}
		else if (textu == 3)
		{
			glTexCoord2f(0.0f, 1.0f);
		}
		glVertex3f(a[0], a[1], a[2]);
		if (textu == 1)
		{
			glTexCoord2f(1.0f, 1.0f);
		}
		else if (textu == 2)
		{
			glTexCoord2f(1.0f, 0.25f);
		}
		else if (textu == 3)
		{
			glTexCoord2f(0.0f, 0.0f);
		}
		glVertex3f(b[0], b[1], b[2]);
		if (textu == 1)
		{
			glTexCoord2f(0.0f, 1.0f);
		}
		else if (textu == 2)
		{
			glTexCoord2f(0.0f, 0.25f);
		}
		else if (textu == 3)
		{
			glTexCoord2f(1.0f, 0.0f);
		}
		glVertex3f(c[0], c[1], c[2]);
		if (textu == 1)
		{
			glTexCoord2f(0.0f, 0.0f);
		}
		else if (textu == 2)
		{
			glTexCoord2f(0.0f, 0.0f);
		}
		else if (textu == 3)
		{
			glTexCoord2f(1.0f, 1.0f);
		}
		glVertex3f(d[0], d[1], d[2]);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glColor3f(1.0f, 1.0f, 1.0f);

	}
}

void dibujaCono(float radio, int lados, float altura, int modoRender)
{
	float ang;
	float a[3], b[3], c[3];
	float delta;

	CVector normalVert[3];
	CVector vec1, vec2, N;

	delta = 360.0f / lados;

	for (int i = 0; i < lados; i++)
	{
		ang = (i + 1)*delta;

		a[0] = radio*(float)cos(ang*PI / 180.0f);
		a[1] = 0.0f;
		a[2] = radio*(float)sin(ang*PI / 180.0f);

		ang = i*delta;

		b[0] = radio*(float)cos(ang*PI / 180.0f);
		b[1] = 0.0f;
		b[2] = radio*(float)sin(ang*PI / 180.0f);

		c[0] = 0.0f;
		c[1] = altura;
		c[2] = 0.0f;

		//C�lculo de las normales
		vec1 = CVector(b[0], b[1], b[2]) - CVector(a[0], a[1], a[2]);
		vec2 = CVector(c[0], c[1], c[2]) - CVector(a[0], a[1], a[2]);
		N = Normaliza(Cruz(vec1, vec2));

		normalVert[0] = Normaliza(CVector(a[0], a[1], a[2]) - CVector(0.0f, 0.0f, 0.0f));
		normalVert[1] = Normaliza(CVector(b[0], b[1], b[2]) - CVector(0.0f, 0.0f, 0.0f));
		normalVert[2] = Normaliza(CVector(c[0], c[1], c[2]) - CVector(0.0f, 0.0f, 0.0f));

		normalVert[0].y = N.y;
		normalVert[1].y = N.y;
		normalVert[2].y = N.y;

		normalVert[0] = Normaliza(normalVert[0]);
		normalVert[1] = Normaliza(normalVert[1]);
		normalVert[2] = Normaliza(normalVert[2]);

		glPushMatrix();
		if (modoRender == 1) glBegin(GL_TRIANGLES);// s�lido
		else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
		glNormal3f(normalVert[0].x, normalVert[0].y, normalVert[0].z);
		glVertex3f(a[0], a[1], a[2]);
		glNormal3f(normalVert[1].x, normalVert[1].y, normalVert[1].z);
		glVertex3f(b[0], b[1], b[2]);
		glNormal3f(normalVert[2].x, normalVert[2].y, normalVert[2].z);
		glVertex3f(c[0], c[1], c[2]);
		glEnd();

		//Tapa inferior
		glColor3f(0.0f, 0.0f, 1.0f);

		if (modoRender == 1) glBegin(GL_TRIANGLES);// s�lido
		else if (modoRender == 2) glBegin(GL_LINE_LOOP);// alambrado
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(b[0], b[1], b[2]);
		glVertex3f(a[0], a[1], a[2]);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glEnd();

		glPushMatrix();
		glTranslatef(-radio-0.35, 0, -radio-0.35);
		dibujaCaja(2*radio + 0.7, 0.5, 2*radio+ 0.7, 1);
		glPopMatrix();
		glPopMatrix();
	}
}



void dibujaHidrante() {

    SeleccionaMaterial(2);
	//Cuerpo del Hidrante
	glPushMatrix();
	glTranslatef(0.0, 0.5f, 0.0f);
	dibujaCilindro(1.0f, 12.0f, 3.0f, 1, 1);
	glPopMatrix();

        SeleccionaMaterial(2);
	//Base
	dibujaCilindro(1.5f, 12.0f, 0.5f, 1, 2);
        
        SeleccionaMaterial(2);
	//Brazo A
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(4.5f, 2.0f, 0.0f);
	dibujaCilindro(1.0f, 12.0f, 2.7f, 1, 3);
	glPopMatrix();

        SeleccionaMaterial(2);
        //Brazo B
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(-4.5f, 2.0f, 0.0f);
	dibujaCilindro(1.0f, 12.0f, 2.7f, 1, 3);
	glPopMatrix();

        SeleccionaMaterial(2);
        //Arriba hidrante adorno
	glPushMatrix();
	glTranslatef(0.0, 3.5, 0.0);
	dibujaCilindro(1.2f, 12.0f, 0.2f, 1, 2);
	glPopMatrix();

        SeleccionaMaterial(2);
	//Semiesfera arriba hidrante
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(0.0, 3.7, 0.0);
	dibujaEsfera(1.0f, 12, 10, 1, 1);
	glPopMatrix();

        SeleccionaMaterial(2);
	//Arriba hidrante adorno
	glPushMatrix();
	glTranslatef(0.0, 4.6, 0.0);
	dibujaCilindro(0.2, 12.0f, 0.2f, 1, 2);
	glPopMatrix();

        SeleccionaMaterial(0);
}

void dibujaEscalera() //se arma con traslaciones 
{

	int codigo = 0x0007;
	//dibujaCaja(ancho, altura, largo, modo render)
	glPushMatrix();//Escalon 1
	glTranslatef(-51.5f, 5.7f, 2.7f);
	dibujaCaja(0.5f, 0.2f, 2.2f, 1);
	glPopMatrix();

	glPushMatrix(); //Escalon 2
	glTranslatef(-50.9f, 6.3f, 2.7f);
	dibujaCaja(0.5f, 0.2f, 2.2f, 1);
	glPopMatrix();

	glPushMatrix(); //Escalon 3
	glTranslatef(-50.3f, 6.8f, 2.7f);
	dibujaCaja(0.5f, 0.2f, 2.2f, 1);
	glPopMatrix();

	glPushMatrix(); //Escalon 4
	glTranslatef(-49.7f, 7.4f, 2.7f);
	dibujaCaja(0.5f, 0.2f, 2.2f, 1);
	glPopMatrix();

	glPushMatrix(); //Escalon 5
	glTranslatef(-49.1f, 8.0f, 2.7f);
	dibujaCaja(0.5f, 0.2f, 2.2f, 1);
	glPopMatrix();

	glPushMatrix(); //Base lado izquierdo
	glTranslatef(-44.0f, 5.2f, 0.0f);
	dibujaCaja(0.2f, 0.5f, 5.0f, 1);
	glPopMatrix();


	glPushMatrix(); //Base lado derecho
	glTranslatef(-56.5f, 5.2f, 0.0f);
	dibujaCaja(0.2f, 0.5f, 5.0f, 1);
	glPopMatrix();

	glPushMatrix(); //Base enfrente izquierdo
	glTranslatef(-56.5f, 5.2f, 5.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	dibujaCaja(0.2f, 0.5f, 5.0f, 1);
	glPopMatrix();

	glPushMatrix(); //Base enfrente derecho
	glTranslatef(-48.3f, 5.2f, 5.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	dibujaCaja(0.2f, 0.5f, 4.5f, 1);
	glPopMatrix();

	glPushMatrix();//Base enfrente derecho dentro
	glTranslatef(-48.3f, 5.2f, 2.5f);
	dibujaCaja(0.2f, 0.5f, 2.5f, 1);
	glPopMatrix();


	glPushMatrix(); //Base centro enfrente dentro
	glTranslatef(-51.5f, 5.2f, 2.7f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	dibujaCaja(0.2f, 0.5f, 3.4f, 1);
	glPopMatrix();

	glPushMatrix(); //Base escalera dentro 
	glTranslatef(-51.5f, 5.2f, 2.7f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
	dibujaCaja(0.2f, 0.5f, 6.4f, 1);
	glPopMatrix();

	glPushMatrix(); //Base escalera fuera
	glTranslatef(-51.5f, 5.2f, 5.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
	dibujaCaja(0.2f, 0.5f, 6.4f, 1);
	glPopMatrix();

	glPushMatrix(); //Barandal derecho atras
	glTranslatef(-43.85f, 5.2f, 0.2f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal derecho centro
	glTranslatef(-43.85f, 5.2f, 2.6f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal derecho frente
	glTranslatef(-43.85f, 5.2f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal frente 1
	glTranslatef(-45.9f, 5.2f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal frente 2
	glTranslatef(-48.2f, 5.2f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal frente 3
	glTranslatef(-51.8f, 5.2f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal frente 4
	glTranslatef(-54.2f, 5.2f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal izquerda frente
	glTranslatef(-56.4f, 5.2f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal izquierda medio
	glTranslatef(-56.4f, 5.2f, 2.6f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal izquierda atr�s
	glTranslatef(-56.4f, 5.2f, 0.2f);
	dibujaCilindro(0.1f, 30, 1.8f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal arriba izquierda
	glTranslatef(-56.4f, 7.0f, 0.0f);
	glRotatef(90, 1.0, 0.0, 0.0);
	dibujaCilindro(0.1f, 30, 5.0f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal arriba derecha
	glTranslatef(-43.85f, 7.0f, 0.0f);
	glRotatef(90, 1.0, 0.0, 0.0);
	dibujaCilindro(0.1f, 30, 5.0f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal arriba frente derecha
	glTranslatef(-43.8f, 7.0f, 4.9f);
	glRotatef(90, 0.0, 0.0, 1.0);
	dibujaCilindro(0.1f, 30, 4.5f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal arriba frente izquierda
	glTranslatef(-56.4f, 7.0f, 4.9f);
	glRotatef(-90, 0.0, 0.0, 1.0);
	dibujaCilindro(0.1f, 30, 4.7f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal adorno izquierda
	glTranslatef(-56.4f, 6.5f, 0.0f);
	glRotatef(90, 1.0, 0.0, 0.0);
	dibujaCilindro(0.1f, 30, 5.0f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal adorno derecha
	glTranslatef(-43.85f, 6.5f, 0.0f);
	glRotatef(90, 1.0, 0.0, 0.0);
	dibujaCilindro(0.1f, 30, 5.0f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal adorno frente derecha
	glTranslatef(-43.8f, 6.5f, 4.9f);
	glRotatef(90, 0.0, 0.0, 1.0);
	dibujaCilindro(0.1f, 30, 4.5f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal adorno frente izquierda
	glTranslatef(-56.4f, 6.5f, 4.9f);
	glRotatef(-90, 0.0, 0.0, 1.0);
	dibujaCilindro(0.1f, 30, 4.7f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal inclinado
	glTranslatef(-51.8f, 6.9f, 4.9f);
	glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
	dibujaCilindro(0.1f, 30, 6.4f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal inclinado adorno
	glTranslatef(-51.8f, 6.4f, 4.9f);
	glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
	dibujaCilindro(0.1f, 30, 6.3f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal frente inclinado 1
	glTranslatef(-50.32f, 6.5f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.9f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal frente inclinado 2
	glTranslatef(-48.84f, 8.0f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.9f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Barandal frente inclinado 3
	glTranslatef(-47.3f, 9.5f, 4.9f);
	dibujaCilindro(0.1f, 30, 1.9f, 1, 0);
	glPopMatrix();

	glPushMatrix(); //Plano izquierdo

	glEnable(GL_TEXTURE_2D);
	glColor3f(0.4f, 0.4f, 0.0f);
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
		glBindTexture(GL_TEXTURE_2D, textura[19].texID);
		glBegin(GL_QUADS);
			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f(0.0,0.0); glVertex3f(-56.6, 0.0, 1.3);
			glTexCoord2f(1.0,0.0); glVertex3f(-56.6, 0.0, 3.3);
			glTexCoord2f(1.0,3.0); glVertex3f(-56.6, 12.5, 3.3);
			glTexCoord2f(0.0,3.0); glVertex3f(-56.6, 12.5, 1.3);
		glEnd();
	glDisable(GL_ALPHA_TEST);
	
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
		glBindTexture(GL_TEXTURE_2D, textura[19].texID);
		glBegin(GL_QUADS);
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f(0.0,0.0); glVertex3f(-56.6, 0.0, 1.3);
			glTexCoord2f(0.0,3.0); glVertex3f(-56.6, 12.5, 1.3);
			glTexCoord2f(1.0,3.0); glVertex3f(-56.6, 12.5, 3.3);
			glTexCoord2f(1.0,0.0); glVertex3f(-56.6, 0.0, 3.3);
		glEnd();
	glDisable(GL_ALPHA_TEST);

	glPopMatrix();

	glPushMatrix(); //Plano derecho
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
		glBindTexture(GL_TEXTURE_2D, textura[19].texID);
		glBegin(GL_QUADS);
			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-43.6, 0.0, 1.3);
			glTexCoord2f(1.0, 0.0); glVertex3f(-43.6, 0.0, 3.3);
			glTexCoord2f(1.0, 3.0); glVertex3f(-43.6, 12.5, 3.3);
			glTexCoord2f(0.0, 3.0); glVertex3f(-43.6, 12.5, 1.3);
		glEnd();
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();


	glPushMatrix(); 
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
		glBindTexture(GL_TEXTURE_2D, textura[19].texID);
		glBegin(GL_QUADS);
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-43.6, 0.0, 1.3);
			glTexCoord2f(0.0, 3.0); glVertex3f(-43.6, 12.5, 1.3);
			glTexCoord2f(1.0, 3.0); glVertex3f(-43.6, 12.5, 3.3);
			glTexCoord2f(1.0, 0.0); glVertex3f(-43.6, 0.0, 3.3);
		glEnd();
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();

	glPushMatrix(); //Plano grande
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
		glBindTexture(GL_TEXTURE_2D, textura[17].texID);
		glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(3.0, 0.0); glVertex3f(-56.4, 5.2, 0.0);
			glTexCoord2f(3.0, 1.0); glVertex3f(-56.4, 5.2, 2.7);
			glTexCoord2f(0.0, 1.0); glVertex3f(-43.85, 5.2, 2.7);
			glTexCoord2f(0.0, 0.0); glVertex3f(-43.85, 5.2, 0.0);
		glEnd();
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();

	glPushMatrix(); //Plano izquierda
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
		glBindTexture(GL_TEXTURE_2D, textura[17].texID);
		glBegin(GL_QUADS);
			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(-56.4, 5.2, 2.7);
			glTexCoord2f(1.0, 1.0); glVertex3f(-56.4, 5.2, 5.0 );
			glTexCoord2f(0.0, 1.0); glVertex3f(-51.5, 5.2, 5.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-51.5, 5.2, 2.7);
		glEnd();
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();

	glPushMatrix(); //Plano derecha
	glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1); //Para quitar el fondo
		glBindTexture(GL_TEXTURE_2D, textura[17].texID);
		glBegin(GL_QUADS);
		glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(-48.3, 5.2, 2.7);
			glTexCoord2f(1.0, 1.0); glVertex3f(-48.3, 5.2, 5.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-43.85, 5.2, 5.0);
			glTexCoord2f(0.0, 0.0); glVertex3f(-43.85, 5.2, 2.7);
		glEnd();
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void dibujaPersonaje()
{
	//Dibuja torso (nodo raiz de la jerarqu�a)   //(datos del modelo, texturas que utiliza el modelo, modo render);
	glPushMatrix();
	glTranslatef(Xtor, Ytor, Ztor);
	glRotatef(Angt2, 0.0f, 1.0f, 0.0f);
	glRotatef(Angt1, 1.0f, 0.0f, 0.0f);
	g_Load3ds.Render3DSFile(&g_3DModel1g, textureModel1g, 1);

	//Cabeza
	glPushMatrix();
	glTranslatef(0.0, 2.5, 0.0);
	glRotatef(Angc2, 0.0f, 1.0f, 0.0f);
	glRotatef(Angc1, 1.0f, 0.0f, 0.0f);
	g_Load3ds.Render3DSFile(&g_3DModel2g, textureModel2g, 1);
	glPopMatrix();

	//Brazo izquierdo
	glPushMatrix();
	glTranslatef(0.9f, 1.8f, 0.0f);
	glRotatef(Angbi3, 0.0f, 0.0f, 1.0f);
	glRotatef(Angbi2, 0.0f, 1.0f, 0.0f);
	glRotatef(Angbi1, 1.0f, 0.0f, 0.0f);
	g_Load3ds.Render3DSFile(&g_3DModel6g, textureModel6g, 1);
	glPopMatrix();
	
	//Pierna derecha
	glPushMatrix();
	glTranslatef(-0.7f, 0.2f, 0.0f);
	glRotatef(Angpder, 1.0f, 0.0f, 0.0f);
	g_Load3ds.Render3DSFile(&g_3DModel4g, textureModel4g, 1);

		//Pie derecho
		glPushMatrix();
		glTranslatef(-0.0f, -1.0f, 0.0f);
		glRotatef(Angpd, 1.0f, 0.0f, 0.0f);
		g_Load3ds.Render3DSFile(&g_3DModel7g, textureModel7g, 1);
		glPopMatrix();

	glPopMatrix();
	
	//Brazo derecho
	glPushMatrix();
	glTranslatef(-0.9f, 1.8f, 0.0f);
	glRotatef(Angbd3, 0.0f, 0.0f, 1.0f);
	glRotatef(Angbd2, 0.0f, 1.0f, 0.0f);
	glRotatef(Angbd1, 1.0f, 0.0f, 0.0f);
	g_Load3ds.Render3DSFile(&g_3DModel5g, textureModel5g, 1);

		//Tabla
		glPushMatrix();
		glTranslatef(-0.7f, -1.7f, 0.0f);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		g_Load3ds.Render3DSFile(&g_3DModel8g, textureModel8g, 1);
		glPopMatrix();

	glPopMatrix();

	//Pierna izquierda
	glPushMatrix();
	glTranslatef(0.7f, 0.2f, 0.0f);
	glRotatef(Angpizq, 1.0f, 0.0f, 0.0f);
	g_Load3ds.Render3DSFile(&g_3DModel3g, textureModel3g, 1);

		//Pie izquierdo
		glPushMatrix();
		glTranslatef(0.0f, -1.0f, 0.0f);
		glRotatef(Angpi, 1.0f, 0.0f, 0.0f);
		g_Load3ds.Render3DSFile(&g_3DModel7g, textureModel7g, 1);
		glPopMatrix();
	glPopMatrix();

glPopMatrix();
}

void actualizaLuzParam()
{
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Posicion de la luz0
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);		// Componente ambiente
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);		// Componente difusa
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);
}

void DibujaLuz()
{
	//Dibuja una esfera que representa la fuente luminosa
	glDisable(GL_LIGHTING);			// Deshabilita iluminaci�n

	glPushMatrix();
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);	// Traslada a la posicion de la luz

	glColor3f(1.0f, 1.0f, 0.0f);	// Color amarillo
	gluSphere(e, 1.0f, 16, 8);	// Dibuja la esfera
	glPopMatrix();


	glEnable(GL_LIGHTING);			// Habilita Iluminaci�n
	glColor3f(1.0f, 1.0f, 1.0f);
}

int RenderizaEscena(GLvoid)								// Aqui se dibuja todo lo que aparecera en la ventana
{
        static float despX = 0.0f;
        static float scaleXY = 0.0f;
        static float ang = 0.0f;
        static int dir = 1;
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
	
	actualizaLuzParam();
	DibujaEjes();
	dibujaEscenario();
        SeleccionaMaterial(1);

		glPushMatrix();
		glTranslatef(2.0, 2.5, 3.0);
		dibujaPersonaje();
		glPopMatrix();

        SeleccionaMaterial(0);

	glEnable(GL_NORMALIZE);
  	dibujaEscalera();
	glDisable(GL_NORMALIZE);

	//Conos 
	glEnable(GL_NORMALIZE);
        SeleccionaMaterial(1);
        //Se traslada el cono a la posicion que se desea
        //Se escala el cono ya que el modelo original es algo grande
	glPushMatrix();
		glTranslatef(-100.0f, 0.0f, -5.0f);
		dibujaCono(1.5,8,4,1);
	glPopMatrix();

        SeleccionaMaterial(1);
        glPushMatrix();
            glTranslatef(8.0f, 0.0f, 3.0f);
      
			dibujaCono(1.5, 8, 4, 1);
        glPopMatrix();

        SeleccionaMaterial(1);
	glPushMatrix();
		glTranslatef(-40.0,0.0,3.0f);
				dibujaCono(1.5, 8, 4, 1);
	glPopMatrix();
        
	SeleccionaMaterial(1);
	glPushMatrix();
		glTranslatef(115.0, 0.0, 15.0f);
				dibujaCono(1.5, 8, 4, 1);
	glPopMatrix();
        
	SeleccionaMaterial(1);
	glPushMatrix();
		glTranslatef(48.0, 0.0, 15.0f);
				dibujaCono(1.5, 8, 4, 1);
	glPopMatrix();
      
        SeleccionaMaterial(1);
        glPushMatrix();
		glTranslatef(-118.0, 0.0, 3.0f);
                glScalef(0.7f, 0.7f, 0.7f);
				dibujaCono(1.5, 8, 4, 1);
	glPopMatrix();
        SeleccionaMaterial(0);
	glDisable(GL_NORMALIZE);
	
	//Hidrante
	glEnable(GL_NORMALIZE);

	glPushMatrix();
		glTranslatef(-100.0, 0.0, 3.0);
		dibujaHidrante();
	glPopMatrix();


	glPushMatrix();
		glTranslatef(127.0, 0.0, -10.0);
		glRotatef(90, 0.0, 1.0, 0.0);
		dibujaHidrante();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-15.0, 0.0, 15.0);
		dibujaHidrante();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(77.0, 0.0, 3.0);
		dibujaHidrante();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(45.0, 0.0, -27.0);
		dibujaHidrante();
	glPopMatrix();

	glDisable(GL_NORMALIZE);

	glPushMatrix();
		glTranslatef(datColadera.pos.x, datColadera.pos.y, datColadera.pos.z);
		glRotatef(datColadera.ang, 0.0f, 0.0f, 1.0f);
		dibujaCilindro(2.0f, 14, 0.4f, 1,0);
	glPopMatrix();
	
	controlEstadosColadera();


	sprintf(strBuffer, "Tipo c�mara: %d", tipoCamara);
	SetWindowText(hWnd, strBuffer);

	DibujaLuz();

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
			MessageBox(NULL,"Falla al liberar DC y RC.","Error de finalizaci�n",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Si no se puede eliminar el RC?
		{
			MessageBox(NULL,"Falla al liberar el contexto de renderizado.","Error de finalizaci�n",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Se pone RC en NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Si no se puede eliminar el DC
	{
		MessageBox(NULL,"Falla al liberar el contexto de renderizado.","Error de finalizaci�n",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Se pone DC en NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Si no se puede destruir la ventana
	{
		MessageBox(NULL,"No se pudo liberar hWnd.","Error de finalizaci�n",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Se pone hWnd en NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Si no se puede eliminar el registro de la clase
	{
		MessageBox(NULL,"No se pudo eliminar el registro de la clase.","Error de finalizaci�n",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Se pone hInstance en NULL
	}
}

//	Este c�digo crea la ventana de OpenGL.  Par�metros:					
//	title			- Titulo en la parte superior de la ventana			
//	width			- Ancho de la ventana								
//	height			- Alto de la ventana								
//	bits			- N�mero de bits a usar para el color (8/16/24/32)	
  
BOOL CreaVentanaOGL(char* title, int width, int height, int bits)
{
	GLuint	PixelFormat;				// Guarda el resultado despues de determinar el formato a usar
	WNDCLASS	wc;						// Estructura de la clase ventana
	DWORD		dwExStyle;				// Estilo extendido de ventana
	DWORD		dwStyle;				// Estilo de ventana
	RECT		WindowRect;				// Guarda los valores Superior Izquierdo / Inferior Derecho del rect�ngulo
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
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Carga el �cono por defecto
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Carga el puntero de flecha
	wc.hbrBackground	= NULL;									// No se requiere ningun fondo
	wc.lpszMenuName		= NULL;									// No hay men� en la ventana
	wc.lpszClassName	= "OpenGL";								// Fija el nombre de la clase.

	if (!RegisterClass(&wc))									// Intenta registrar la clase de ventana
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											
	}
		
	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;					// Estilo extendido de ventana
	dwStyle=WS_OVERLAPPEDWINDOW;									// Estilo de ventana

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Ajusta la ventana al tama�o especificado

	// Crea la ventana
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Estilo extendido para la ventana
								"OpenGL",							// Nombre de la clase
								title,								// T�tulo de la ventana
								dwStyle |							// Definici�n del estilo de la ventana
								WS_CLIPSIBLINGS |					// Estilo requerido de la ventana
								WS_CLIPCHILDREN,					// Estilo requerido de la ventana
								0, 0,								// Posici�n de la ventana
								WindowRect.right-WindowRect.left,	// Calcula el ancho de la ventana
								WindowRect.bottom-WindowRect.top,	// Calcula el alto de la ventana
								NULL,								// No hay ventana superior
								NULL,								// No hay men�
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

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Si Windows no encontr� un formato de pixel compatible
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
		MessageBox(NULL,"Falla en la inicializaci�n.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	return TRUE;							// Todo correcto
}

LRESULT CALLBACK WndProc(	HWND	hWnd,	// Manejador para esta ventana
							UINT	uMsg,	// Mensaje para esta ventana
							WPARAM	wParam,	// Informaci�n adicional del mensaje
							LPARAM	lParam)	// Informaci�n adicional del mensaje
{
	switch (uMsg)							// Revisa los mensajes de la ventana
	{
		case WM_ACTIVATE:					// Revisa el mensaje de activaci�n de ventana
		{
			if (!HIWORD(wParam))			// Revisa el estado de minimizaci�n
			{
				active=TRUE;				// El programa est� activo
			}
			else
			{
				active=FALSE;				// El programa no est� activo
			}

			return 0;						// Regresa al ciclo de mensajes
		}

		case WM_SYSCOMMAND:					// Intercepta comandos del sistema
		{
			switch (wParam)					// Revisa llamadas del sistema
			{
				case SC_SCREENSAVE:			// �Screensaver tratando de iniciar?
				case SC_MONITORPOWER:		// �Monitor tratando de entrar a modo de ahorro de energ�a?
				return 0;					// Evita que suceda
			}
			break;							// Sale del caso
		}

		case WM_CLOSE:						// Si se recibe un mensaje de cerrar...
		{
			PostQuitMessage(0);				// Se manda el mensaje de salida
			return 0;						// y se regresa al ciclo
		}

		case WM_KEYDOWN:					// Si se est� presionando una tecla...
		{
			keys[wParam] = TRUE;			// Si es as�, se marca como TRUE
			return 0;						// y se regresa al ciclo
		}

		case WM_KEYUP:						// �Se ha soltado una tecla?
		{
			keys[wParam] = FALSE;			// Si es as�, se marca como FALSE
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

// Este es el punto de entrada al programa; la funci�n principal 
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instancia
					HINSTANCE	hPrevInstance,		// Instancia previa
					LPSTR		lpCmdLine,			// Parametros de la linea de comandos
					int			nCmdShow)			// Muestra el estado de la ventana
{
	MSG		msg;									// Estructura de mensajes de la ventana
	BOOL	done=FALSE;								// Variable booleana para salir del ciclo

	// Crea la ventana OpenGL
	if (!CreaVentanaOGL("Laboratorio de Computaci�n Gr�fica",640,480,16))
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
			if (active)								// Si est� activo el programa...
			{
				if (keys[VK_ESCAPE])				// Si se ha presionado ESC
				{
					done=TRUE;						// ESC indica el termino del programa
				}
				else								// De lo contrario, actualiza la pantalla
				{
					RenderizaEscena();				// Dibuja la escena, se puede seleccionar la escena cambiando la llamada
					SwapBuffers(hDC);				// Intercambia los Buffers (Double Buffering)
					if (play)
						animacion(20); //A mayor cantidad de frames es mas lento.

				}

				if(!ManejaTeclado()) return 0;
			}
			
		}
	}
        DescargaModelos();
		DescargaTexturas();
	// Finalizaci�n del programa
	DestruyeVentanaOGL();							// Destruye la ventana
	return (msg.wParam);							// Sale del programa
}

// Agregar variables para cambiar de lugar la camara
int ManejaTeclado()
{
	

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

        if (keys[VK_NEXT] || keys['L'])
	{
		controlCamara(6);
	}

        if (keys[VK_HOME] || keys['I'])
	{
		controlCamara(7);
	}

        if (keys[VK_END] || keys['K'])
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


        if ((GetAsyncKeyState('X') & 1) == 1)
        {
            if (IncDec == 0)
                IncDec = 1;
            else if (IncDec == 1)
                IncDec = 0;
        }

        //Ang. 1 Torso
        if (keys['1'])
        {
            if (IncDec == 0)
                Angt1 += 1.0f;
            else if (IncDec == 1)
                Angt1 -= 1.0f;
        }

        //Ang. 2 Torso
        if (keys['2'])
        {
            if (IncDec == 0)
                Angt2 += 1.0f;
            else if (IncDec == 1)
                Angt2 -= 1.0f;
        }

        //Ang. 1 Cabeza
        if (keys['3'])
        {
            if (IncDec == 0)
                Angc1 += 1.0f;
            else if (IncDec == 1)
                Angc1 -= 1.0f;
        }

        //Ang. 2 Cabeza
        if (keys['4'])
        {
            if (IncDec == 0)
                Angc2 += 1.0f;
            else if (IncDec == 1)
                Angc2 -= 1.0f;
        }

        //Ang. 1 Brazo izq
        if (keys['5'])
        {
            if (IncDec == 0)
                Angbi1 += 1.0f;
            else if (IncDec == 1)
                Angbi1 -= 1.0f;
        }

        //Ang. 2 Brazo izq
        if (keys['6'])
        {
            if (IncDec == 0)
                Angbi2 += 1.0f;
            else if (IncDec == 1)
                Angbi2 -= 1.0f;
        }

        //Ang. 3 Brazo izq
        if (keys['7'])
        {
            if (IncDec == 0)
                Angbi3 += 1.0f;
            else if (IncDec == 1)
                Angbi3 -= 1.0f;
        }

        //Ang. 1 Brazo der
        if (keys['8'])
        {
            if (IncDec == 0)
                Angbd1 += 1.0f;
            else if (IncDec == 1)
                Angbd1 -= 1.0f;
        }

        //Ang. 2 Brazo der
        if (keys['9'])
        {
            if (IncDec == 0)
                Angbd2 += 1.0f;
            else if (IncDec == 1)
                Angbd2 -= 1.0f;
        }

        //Ang. 3 Brazo der
        if (keys['0'])
        {
            if (IncDec == 0)
                Angbd3 += 1.0f;
            else if (IncDec == 1)
                Angbd3 -= 1.0f;
        }

        //Ang. Pierna izq
        if (keys['Q'])
        {
            if (IncDec == 0)
                Angpizq += 1.0f;
            else if (IncDec == 1)
                Angpizq -= 1.0f;
        }

        //Ang. Pierna der
        if (keys['W'])
        {
            if (IncDec == 0)
                Angpder += 1.0f;
            else if (IncDec == 1)
                Angpder -= 1.0f;
        }

        //Ang. Pie izq
        if (keys['E'])
        {
            if (IncDec == 0)
                Angpi += 1.0f;
            else if (IncDec == 1)
                Angpi -= 1.0f;
        }

        //Ang. Pie der
        if (keys['R'])
        {
            if (IncDec == 0)
                Angpd += 1.0f;
            else if (IncDec == 1)
                Angpd -= 1.0f;
        }

        //Pos. X torso
        if (keys['T'])
        {
            if (IncDec == 0)
                Xtor += 1.0f;
            else if (IncDec == 1)
                Xtor -= 1.0f;
        }

        //Pos. Y torso
        if (keys['Y'])
        {
            if (IncDec == 0)
                Ytor += 1.0f;
            else if (IncDec == 1)
                Ytor -= 1.0f;
        }

        //Pos. Z torso
        if (keys['U'])
        {
            if (IncDec == 0)
                Ztor += 1.0f;
            else if (IncDec == 1)
                Ztor -= 1.0f;
        }

		//Controles de la iluminaci�n
		if (keys['X'])
			LightPos[0] += 1.0f; //Hacia la derecha

		if (keys['Z'])
			LightPos[0] -= 1.0f; //Hacia la izquierda

		if (keys['C'])
			LightPos[1] += 1.0f; //Hacia arriba

		if (keys['V'])
			LightPos[1] -= 1.0f; //Hacia abajo

		if (keys['B'])
			LightPos[2] += 1.0f; //Hacia adelante

		if (keys['N'])
			LightPos[2] -= 1.0f; //Hacia atr�s

		if (GetKeyState(VK_SPACE) & 0x80)
		{
			if (play == false)
			{
				//Se le asignan a las variables del personaje los 
				//valores almacenados en el primer keyframe para que
				//inicie desde ah� la animaci�n.
				Angt1 = KeyFrame[0].Angt1;
				Angt2 = KeyFrame[0].Angt2;
				Angc1 = KeyFrame[0].Angc1;
				Angc2 = KeyFrame[0].Angc2;
				Angbi1 = KeyFrame[0].Angbi1;
				Angbi2 = KeyFrame[0].Angbi2;
				Angbd1 = KeyFrame[0].Angbd1;
				Angbd2 = KeyFrame[0].Angbd2;
				Angpizq = KeyFrame[0].Angpizq;
				Angpder = KeyFrame[0].Angpder;
				Angpi = KeyFrame[0].Angpi;
				Angpd = KeyFrame[0].Angpd;
				Xtor = KeyFrame[0].Xtor;
				Ytor = KeyFrame[0].Ytor;
				Ztor = KeyFrame[0].Ztor;

				play = true;
				playIndex = 0;

			}
		}

		if (keys['O'])
			datColadera.estado = 1;

	return TRUE;
}