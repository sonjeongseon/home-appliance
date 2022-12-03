#include"testgraph.h"


/* Main function */
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(1000, 300);
	glutCreateWindow("CO2, TEM, HUM");

	/* Register GLUT callbacks. */
	glutDisplayFunc(redraw);
	glutKeyboardFunc(key);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	thread thread1(getData);
	/* Init the GL state */
	glLineWidth(1.0);

	/* Main loop */
	glutMainLoop();
	return 0;
}