#include <iostream>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib> 
#include <ctime>
#include <fstream>
#include <random>

using namespace std;
void afficher(char tableau[][7], int ligne) {
	cout << "-----------------------------" << endl;
	for (int l = ligne - 1; l >= 0; l--) {
		for (int c = 0; c < 7; c++) {
			cout << "| " << tableau[l][c] << " ";
		}
		cout << "| ";
		cout << endl << "-----------------------------" << endl;
	}
	cout << "  1   2   3   4   5   6   7" << endl;
}

bool gagner(char tableau[][7], int ligne, int codeAscii) {

	for (int i = 0; i < 6; i++) {                     	//to do utiliser ligne, pour la reutilisabilite
		for (int j = 0; j < 7; j++) {
			if (tableau[i][j] == codeAscii) {
				if (i <= 2) {							//vertical
					if (tableau[i + 3][j] == codeAscii && tableau[i + 2][j] == codeAscii && tableau[i + 1][j] == codeAscii) { return true; }
				}
				if (j <= 3) {							//horizontale
					if (tableau[i][j + 3] == codeAscii && tableau[i][j + 2] == codeAscii && tableau[i][j + 1] == codeAscii) { return true; }
				}
				if (i <= 2 && j <= 3) { 				//oblique droite
					if (tableau[i + 3][j + 3] == codeAscii && tableau[i + 2][j + 2] == codeAscii && tableau[i + 1][j + 1] == codeAscii) { return true; }
				}
				if (i <= 2 && j >= 3) { 				//oblique gauche
					if (tableau[i + 3][j - 3] == codeAscii && tableau[i + 2][j - 2] == codeAscii && tableau[i + 1][j - 1] == codeAscii) { return true; }
				}
			}
		}
	}
	return false;
}
bool pleine(int tableau[], int taille) {                 	//to do a reverifier
	int pions = 0;
	for (int i = 0; i < taille; i++) {
		pions += tableau[i];
	}
	if (pions < 42) { return false; }   						// 6 * 7 = 42 pions au maximum
	else { return true; }
}
bool coupGagnant(char tableau[][7], int codeAscii, short& colonne) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			if (tableau[i][j] == codeAscii) {
				if (i <= 2) {							//vertical
					if (tableau[i + 3][j] == 32 && tableau[i + 2][j] == codeAscii && tableau[i + 1][j] == codeAscii) { colonne = j; return true; }
				}
				if (j <= 3) {							//horizontale : gauche a droite
					if (tableau[i][j + 3] == 32 && tableau[i][j + 2] == codeAscii && tableau[i][j + 1] == codeAscii) {
						if (i == 0 || (i > 0 && tableau[i - 1][j + 3] != 32)) { //(1)
							colonne = j + 3;
							return true;
						}
					}
				}
				if (j <= 4 && j >= 1) {					//horizontale : droite a gauche
					if (tableau[i][j - 1] == 32 && tableau[i][j + 2] == codeAscii && tableau[i][j + 1] == codeAscii) {
						if (i == 0 || (i > 0 && tableau[i - 1][j - 3] != 32)) {
							colonne = j - 1;
							return true;
						}
					}
				}
				if (i <= 2 && j <= 3) { 				//oblique droite																	//(2)
					if (tableau[i + 3][j + 3] == 32 && tableau[i + 2][j + 2] == codeAscii && tableau[i + 1][j + 1] == codeAscii
						&& (tableau[i + 2][j + 3] != 32)) {
						colonne = j + 3;
						return true;
					}
				}
				if (i <= 2 && j >= 3) { 				//oblique gauche
					if (tableau[i + 3][j - 3] == 32 && tableau[i + 2][j - 2] == codeAscii && tableau[i + 1][j - 1] == codeAscii
						&& (tableau[i + 2][j - 3] != 32)) {
						colonne = j - 3;
						return true;
					}
				}
			}
		}
	}
	return false;
}
bool strategie7(char tableau[][7], short& colonne) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			if (tableau[i][j] == 79) {
				if (tableau[i + 1][j + 1] == 32) {
					colonne = j + 1; 
					return true;
				}
				else if (tableau[i + 1][j + 1] == 79) {
					if (tableau[i + 2][j + 1] == 32) {
						colonne = j + 1;
						return true;
					}
					else if (tableau[i + 2][j + 1] == 79) {
						if (tableau[i + 2][j] == 32) {
							colonne = j;
							return true;
						}
						else if (tableau[i + 2][j] == 79) {
							if (tableau[i + 2][j + 2] == 32 && tableau[i][j + 2] != 32) {
								colonne = j + 2;
								return true;
							}
							else continue;
						}
						else continue;
					}
					else continue;
				}
				else continue;
			}
		}
	}
	return false;
}
bool coupGagnantSpecial(char tableau[][7], int codeAscii, short& colonne) {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			if (tableau[i][j] == codeAscii) {
				//4
				if (j <= 3) { //horizontale
					if (tableau[i][j + 1] == 32 && tableau[i][j + 2] == codeAscii && tableau[i][j + 3] == codeAscii && (i == 0 || tableau[i - 1][j + 1] != 32)) {
						colonne = j + 1;
						return true;
					}
					if (tableau[i][j + 1] == codeAscii && tableau[i][j + 2] == 32 && tableau[i][j + 3] == codeAscii && (i == 0 || tableau[i - 1][j + 2] != 32)) {
						colonne = j + 2;
						return true;
					}
				}
				if (i <= 2 && j <= 3) { 				//oblique droite	
					if (tableau[i + 1][j + 1] == 32 && tableau[i + 2][j + 2] == codeAscii && tableau[i + 3][j + 3] == codeAscii && tableau[i][j + 1] != 32) {
						colonne = j + 1;
						return true;
					}
					if (tableau[i + 1][j + 1] == codeAscii && tableau[i + 2][j + 2] == 32 && tableau[i + 3][j + 3] == codeAscii && tableau[i + 1][j + 2] != 32) {
						colonne = j + 2;
						return true;
					}
				}
				if (i <= 2 && j >= 3) { 					//oblique gauche
					if (tableau[i + 1][j - 1] == 32 && tableau[i + 2][j - 2] == codeAscii && tableau[i + 3][j - 3] == codeAscii && tableau[i][j - 1] != 32) {
						colonne == j - 1;
						return true;
					}
					if (tableau[i + 1][j - 1] == codeAscii && tableau[i + 2][j - 2] == 32 && tableau[i + 3][j - 3] == codeAscii && tableau[i + 1][j - 2] != 32) {
						colonne == j - 2;
						return true;
					}
				}
				if (i == 0 && j <= 4 && j >= 1) { //3
					if (tableau[i][j + 1] == codeAscii && tableau[i][j + 2] == 32 && tableau[i][j - 1] == 32) {
						colonne = j - 1;
						return true;
					}
				}
			}
		}
	}
	return false;
}

short choisirMeilleurCoup(char tableau[][7]) {
	short n;
	if (coupGagnant(tableau, 79, n)) { //si l'ordinateur peut gagner
		//cout << "gagnantO" << endl; 
		return n;
	}
	else if (coupGagnant(tableau, 88, n)) {
		//cout << "gagnantJ" << endl;
		return n;
	}	//si le joueur est sur le point de gagner
	else if (coupGagnantSpecial(tableau, 88, n)) {
		//cout << "specialJ" << endl; 
		return n;
	}
	else if (coupGagnantSpecial(tableau, 79, n)) {
		//cout << "specialO" << endl;
		return n;
	}
	else if (strategie7(tableau, n)) {
		//cout << "strategie7" << endl;
		return n;
	}
	else {
		random_device dev;
		mt19937 rng(dev());
		uniform_int_distribution<std::mt19937::result_type> dist6(0, 6);
		n = dist6(rng);
		//srand(time(0));    				//initialisation de srand() avec l'heure courante 
		//n = rand() % 7;  				//generation de nombre aleatoire 
		//cout << "Random" << endl;
		return n;
	}
}

//fonction pour enregistrer le score dans un fichier on ne la pas utiliser mais ca pourait etre 
//important dans certaines situations
void enregistrerScore(const string& nomFichier, int scoreJoueur, int scoreOrdinateur) {   
    ofstream fichier(nomFichier, ios::app);
    fichier << "Joueur: " << scoreJoueur << ", Ordinateur: " << scoreOrdinateur << endl;
    fichier.close();
}

int main() 
    {
          
    int scoreJoueur = 0, scoreOrdinateur = 0;                                                  //variables pour enregistrer les scores
    char rejouer;                                                                              //variable pour stocker le choix du jouer o/O=oui et  n/N=non
    //boucle pour rejouer autant de fois que le jouer souhaite
    do {                                                                                       
        
        char tableau[6][7] = {  {32,32,32,32,32,32,32}, 
          						{32,32,32,32,32,32,32},		//	32 = space " " 
          						{32,32,32,32,32,32,32},		//	88 = "X" - joueur
          						{32,32,32,32,32,32,32},		//	79 = "O" - ordinateur
         	 					{32,32,32,32,32,32,32},
          						{32,32,32,32,32,32,32} }; 

		int q[2]; 											// Tube du premier fils vers le second
        int p[2]; 											// Tube du second fils vers le premier
		short n = 0 ; 										// La colonne choisie par le joueur ou l' ordinateur
		int T[7]={0};										// Tableau permettant de verifier si une colonne est pleine; chaque colonne est un compteur
        
		pid_t pid1, pid2;
		pid_t PID_Ordi;                                     //PID de l'ordinateur
		pid_t PID_Joueur;                                   //PID du joueur

		if(pipe(q) == -1 || pipe(p) == -1 )					// Échec de création du tube
			exit(-1);
	
		afficher(tableau,6);

		switch(PID_Ordi = fork())
		{
		case -1:
			cout << "Erreur de fork() 1 !\n";
			exit(1);
			break;

		
		case 0:			//Fils #1 Ordinateur
		    close(q[0]); 						            // Fermeture du descripteur de lecture du tube 1 dans le premier fils
            close(p[1]); 									// Fermeture du descripteur d'écriture du tube 2 dans le premier fils

			while(true){

				read(p[0],&n,2);				            //fils 1 tente de lire dans le pipe2 // attend donc que fils 2 ecrive dans ce tube
				
				if (n==200){
					exit(2); 								//fils #2 a gagner, fils #1 se termine avec le code 200 pour signaler le resultat au pere}
				}
				else if (n==300){
					exit(3); 								//egalite
				}
				
				tableau[T[n-1]][n-1]=88 ; 		            //mise a jour du tableau
				T[n-1]++;
				
				n=choisirMeilleurCoup(tableau);
				
				while(T[n]>5){ n= (n+1) % 7;} 	            //si la colonne est pleine, aller a la suivante
				
    			tableau[T[n]][n]=79;  			            //ecriture dans le tableau
    			T[n]++;
    				
    			//system("clear");							
    			afficher(tableau,6);
    				
    			if (gagner(tableau,6,79)){
    				n=100; 						            //100 est le code pour dire que fils #1 a gagner
    				write(q[1],&n,2);
    				exit(1); 					            //fils #1 se termine avec le code 1 pour signaler le resultat au pere
    			}
				else if (pleine(T,7)){
					n=300; 									//300 est le code pour dire a l'autre fils que le tableau est plein i.e egalite
    				write(q[1],&n,2);
    				exit(3); 					            //fils #1 se termine avec le code 3 pour signaler le resultat au pere
    			}
    			write(q[1],&n,2);				            //ecriture de la colonne jouer par l'ordi dans le tube1, communication avec le 2e fils
			}
			break;                                          //Fin fils #1

		
		default:      // Père
			
			switch(PID_Joueur = fork()) 
			{
				case -1:
					cout << "Erreur de fork() 2 !\n";
					exit(-1);
					break;
					
				case 0:										//Fils #2 Joueur
				    close(q[1]); 				            // Fermeture du descripteur d'écriture du tube 1 dans le 2e fils
                    close(p[0]); 				            // Fermeture du descripteur de lecture du tube 2 dans le 2e fils

					while(true){

						while(true){
							cout << "Numero de colonne : ";
							cin >> n;
							if (n < 1 || n>7) {
								cout << "Entree Invalide, vos options sont de 1 a 7 " << endl;
								continue;
							}
							else if (T[n - 1] > 5) {
								cout << "Colonne pleine " << endl;
								continue;
							}
							break;
						}
						
						tableau[T[n - 1]][n - 1] = 88;			// Mise a jour du tableau
						T[n - 1]++;
						//system("clear");						// Effacer l'ancien tableau
    						
						if (gagner(tableau,6,88))
    					{
							afficher(tableau,6);
    						n=200; 		 						// 200 est le code pour dire que fils #2 a gagner
    						write(p[1],&n,2);    
    						exit(2); 							// Fils #2 se termine avec le code 2 pour signaler le resultat au pere
    					}
						else if (pleine(T,7)){
							n=300;								// 300 est le code pour dire a l'autre fils que le tableau est plein i.e egalite
							write(p[1],&n,2);    
    						exit(3); 							// Fils #2 se termine avec le code 2 pour signaler le resultat au pere
						}
    						
    					write(p[1],&n,2); 			            // Ecriture de la colone jouer par le joueur dans le tube2 que fils 1 va lire
    					read(q[0],&n,2); 			            // Lecture de la colonne jouer par l'ordi dans le tube 1
    						
    					if (n==100) {
							exit(1); 				// Fils #1 a gagner, fils #1 se termine avec le code 100 pour signaler le resultat au pere}
						}
						else if (n==300){
							exit(3); 				// Egalite
						}
						
    					tableau[T[n]][n]=79 ; 					// Mise a jour du tableau
						T[n]++;
						
					}
					break;    // Fin fils #2
				
				default:      // Père
                 
					//fermeture de tous les tubes 
					close(q[0]); 
					close(q[1]); 
					close(p[0]); 
					close(p[1]); 
				
					int c1,c2;
					wait(&c1);									// Attends que les fils se terminent
					wait(&c2);
					//cout<<"c1= "<<c1<<"  c2= "<<c2<<endl;		// Verification 
					
				if (c1 == 256){									// Les codes de retour sont multiplie par 256
				
				        ++scoreOrdinateur;                      // Incrementation du score de l'ordi
					cout<<"Vous avez perdu"<<endl;	                           
				}
				else if(c1 == 512){								// Code 2 * 1 0000 0000 = 512 en decimal
				
				        ++scoreJoueur;                          // incrementer le score du joueur
					cout<<"Vous avez gagne"<<endl; 
					
				}
				else{
					cout<<"Egalite"<<endl;                                    
				}
				break;   //Fin pere
			}
	}  
        cout << endl <<"Scores - Joueur: " << scoreJoueur << ", Ordinateur: " << scoreOrdinateur << endl;  //afficher le score du jeu 
        
        cout << "Voulez-vous jouer à nouveau ? (O/N) : ";                                           //incitation de faire un choix pour rejouer ou pas 
        while(true){                                                                                //boucle qui continue tant que le choix est incorrecte
           cin >> rejouer;
           rejouer = toupper(rejouer);                                                               //convertier le caractere en majuscul
           
           if(rejouer=='O' or rejouer=='N') break; 
           cout<<"choix incorrecte! (O/N) : ";                                       
           continue;                     
        }
        
		if (rejouer == 'O');// system("clear");                                                      //si le joueur veux rejouer effacer tout sur le terminal
        
        //enregistrerScore("scores.txt", scoreJoueur, scoreOrdinateur);
        
    } while (rejouer == 'O');

   cout << endl <<"Merci d'avoir joué ! Scores finaux - Joueur: " << scoreJoueur << ", Ordinateur: " << scoreOrdinateur << endl << endl; //message de fin de jeu

    return 0;
}

