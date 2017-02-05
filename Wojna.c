/*Symulator gry karcianej wojna z wykorzystaniem dynamicznych struktur danych.
Bartosz Stawczyk 2017*/

#define _CRT_SECURE_NO_WARNINGS
#define TALIA 52 
#define KARTA 3
#define KOLOR 15 
#define FIGURA 15
#define ZNAKI 15 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
#include <conio.h>

struct karta {
	char kolor[KOLOR];
	char figura[FIGURA];
	int wartosc;
};

struct talia {
	struct talia *prev;
	struct karta card;
};

char*** alokujDwuwymiarowaChar(int pierwszyWymiar, int drugiWymiar, int trzeciWymiar) {
	int i, j;
	char ***tab;
	tab = (char***)calloc(pierwszyWymiar, sizeof(char**));
	for (i = 0; i < pierwszyWymiar; i++) {
		tab[i] = (char**)calloc(drugiWymiar, sizeof(char*));
		for (j = 0; j < drugiWymiar; j++) {
			tab[i][j] = (char*)calloc(trzeciWymiar, sizeof(char));
		}
	}
	return tab;
}

void zwolnijTablicaDwuwymiarowaChar(char ***tab, int pierwszyWymiar, int drugiWymiar) {
	int i, j;
	for (i = 0; i < pierwszyWymiar; i++) {
		for (j = 0; j < drugiWymiar; j++) {
			free(tab[i][j]);
		}
		free(tab[i]);
	}
	free(tab);
}

//wczytywane wartosci wpychane na stos, wartosc, domyœlnie jako, char jest zmieniana na int za pomoc¹ funkcji atoi()
void pushWczytane(struct talia **talia, char *figura, char *kolor, char*wartosc) {
	struct talia *nowy;
	int wartoscInt = 0;
	nowy = malloc(sizeof(struct talia));
	strcpy((*nowy).card.figura, figura);
	strcpy((*nowy).card.kolor, kolor);
	wartoscInt = atoi(wartosc);
	(*nowy).card.wartosc = wartoscInt;
	(*nowy).prev = *talia;
	*talia = nowy;
}

//alokuje pamiêæ na nowy element struktury, wpycha element na stos
void push(struct talia **talia, char *figura, char *kolor, int wartosc) {
	struct talia *nowy;
	int wartoscInt = 0;
	nowy = malloc(sizeof(struct talia));
	strcpy((*nowy).card.figura, figura);
	strcpy((*nowy).card.kolor, kolor);
	(*nowy).card.wartosc = wartosc;
	(*nowy).prev = *talia;
	*talia = nowy;
}

//zdejmij element ze stosu, zwolnij pamiêæ i zapisz wartosci w strukturze karta
void pop(struct talia **talia, struct karta *card) {
	struct talia *temp;
	struct karta wartosci;
	wartosci = (*talia)->card;
	if (*talia == NULL) {
		strcpy((*card).figura, 0);
		strcpy((*card).kolor, 0);
		(*card).wartosc = 0;
		*card = wartosci;
	}
	temp = (*talia)->prev;
	free(*talia);
	*talia = temp;
	*card = wartosci;
}

//wczytuje z pliku .csv i umieszcza w tablicy dynamicznej char
char*** wczytajTalie() {
	FILE *plik;
	int i = 0;
	char ***tab;
	char bufor[50];
	tab = alokujDwuwymiarowaChar(TALIA,KARTA,ZNAKI);
	plik = fopen("talia52.csv", "r");
	while (fscanf(plik, "%s", bufor) != EOF) {
		sscanf(bufor, "%[^;];%[^;];%s", tab[i][0], tab[i][1], tab[i][2]);
		i++;
	}
	fclose(plik);
	return tab;	
}

//funkcja sprawdzaj¹ca czy dana liczba wyst¹pi³a wczeœniej
int czyWystapila(int wylosowana, int* arr, int licznik) {
	int i = 0;
	if (licznik == 0)
		return 0;
	do {
		if (arr[i] == wylosowana)
			return 1;
		i++;
	} while (i < licznik);
	return 0;
}

//funkcja generuj¹ca liczby losowe bez powtórzeñ, umieszczaj¹ca w tablicy statycznej i wpychaj¹ca odpowiadaj¹ce elementy z tablicy 
//char zawieraj¹cej figurê, kolor i wartoœæ karty na stos pierwszego i drugiego gracza na zmianê
void tasujRozdaj(struct talia **A, struct talia **B, char ***tab) {
	int i = 0; //licznik kolejnych komórek tablicy losowo generowanych liczb
	int j = 0; //licznik dla umieszczania kolejnych elementów na stosy
	int arr[TALIA]; //tablica losowo generowanych liczb
	int x = 0; //liczba wygenerowana losowo
	srand(time(NULL));

	//wype³nienie tablicy arr[] liczbami losowymi z przedzia³u od 0 do TALIA-1, czyWystapila() gwarantuje brak powtorzen
	do {
		x = rand() % TALIA;
		if (czyWystapila(x, arr, i) == 0) {
			arr[i] = x;
			i++;
		}
	} while (i < TALIA);

	for (j = 0; j < TALIA; j++) {
		//jeœli liczba podzielna przez dwa dodaje element do stosu A, nieparzysta - dodaje do B
		if (j % 2 == 0) {
			pushWczytane(A, tab[arr[j]][0], tab[arr[j]][1], tab[arr[j]][2]);
//			printf("STOS A: %s %s\n", (*A)->card.figura, (*A)->card.kolor);
		}
		else {
			pushWczytane(B, tab[arr[j]][0], tab[arr[j]][1], tab[arr[j]][2]);
//			printf("STOS B: %s %s\n\n", (*B)->card.figura, (*B)->card.kolor);
		}
	}
}

/*funkcja gwarantuj¹ca opóŸnienia w zale¿noœci od padanej liczby milisekund (int czas);
clock_t przechowuje liczbe cykli zegara;
clock() zwraca liczbê cykli;
CLOCKS__PER_SEC liczba cykli zegara na sekunde;*/
void wait(int czas) {
	clock_t koniec = clock() + czas*CLOCKS_PER_SEC / 1000;
	while (clock() < koniec)
		continue;
}

void wyswietlTabliceDwuwymiarowaChar(char ***tab) {
	int i;
	for (i = 0; i < TALIA; i++) {
		printf("%s %s %s\n", tab[i][0], tab[i][1], tab[i][2]);
	}
}

//wyœwietla elementy w stosie
void wyswietlStos(struct talia* Gracz) {
	int i = 1;
	while (Gracz != NULL) {
		printf("%d.\t%s %s\n",i, (*Gracz).card.figura, (*Gracz).card.kolor);
		Gracz = (*Gracz).prev;
		i++;
	}
	printf("\n");
	system("pause");
}

//zwraca 0 w wypadku tych samych wartosci kart; 1 gdy element ze stosu gracz1 ma wiêksz¹ wartoœæ; 2 w przeciwynym wypadku
int stanRozgrywki(struct karta gracz1, struct karta gracz2) {
	int runda = 0;
	if (gracz1.wartosc== gracz2.wartosc)
		return runda = 0;
	else if (gracz1.wartosc>gracz2.wartosc)
		return runda = 1;
	else
		return runda = 2;	
}

//wyœwietla wartoœci ze szczytowych elementów stosów Gracza1 i Gracza2
void wyswietlRunde(struct talia *gracz1, struct talia *gracz2) {
	printf("Gracz1:\t%s %s\nGracz2:\t%s %s\n", (*gracz1).card.figura, (*gracz1).card.kolor, (*gracz2).card.figura, (*gracz2).card.kolor);
}

//funkcja zdejmuj¹ca po jednej karcie ze stosów graczy i umieszczaj¹ca te karty w odpowiednich stosach-pula
//zwiêksza liczbe rund o 1
void runda(int *liczbaRund , struct talia **gracz1, struct talia **gracz2, struct talia **pulaGracz1, struct talia **pulaGracz2, struct karta *wojna1, struct karta *wojna2) {
	*liczbaRund = *liczbaRund + 1;
	pop(gracz1, wojna1);
	pop(gracz2, wojna2);
	push(pulaGracz1, (*wojna1).figura, (*wojna1).kolor, (*wojna1).wartosc);
	push(pulaGracz2, (*wojna2).figura, (*wojna2).kolor, (*wojna2).wartosc);
}

//funkcja, która do Puli wrzuca karty Gracza wygranego, przerzuca po jedenj karcie ze stosów pulaGracz, pocz¹wszy od gracza przegranego, a¿ do opró¿nienia obu stosów
//przenosi karty z Puli do stosu zwyciêzkiego gracza
void  doWspolnejPuli_naStosWygranego(struct talia **Gracz1, struct talia **Gracz2, struct talia **pulaGracz1, struct talia **pulaGracz2, struct talia **pulaWspolna, struct karta *temp1, struct karta *temp2, int zwyciezca) {
	if (zwyciezca == 1) {
		//przenies karty zwyciezcy na stos pulaWspolna
		while (*Gracz1 != NULL) {
			pop(Gracz1, temp1);
			push(pulaWspolna, temp1->figura, temp1->kolor, temp1->wartosc);
		}
		//przenies po jednej karcie ze stosów kart rzuconych na "stó³", a¿ do opró¿nienia stosów
		//najpierw karta przegranego
		while (*pulaGracz1 != NULL && *pulaGracz2 != NULL) {
			pop(pulaGracz2, temp2);
			push(pulaWspolna, temp2->figura, temp2->kolor, temp2->wartosc);
			pop(pulaGracz1, temp1);
			push(pulaWspolna, temp1->figura, temp1->kolor, temp1->wartosc);
		}
		//przenies z puliWspolnej na stos zwyciêzcy
		while (*pulaWspolna != NULL) {
			pop(pulaWspolna, temp1);
			push(Gracz1, temp1->figura, temp1->kolor, temp1->wartosc);
		}
	}
	else {
		//przenies karty zwyciezcy na stos pulaWspolna
		while (*Gracz2 != NULL) {
			pop(Gracz2, temp1);
			push(pulaWspolna, temp1->figura, temp1->kolor, temp1->wartosc);
		}
		//przenies po jednej karcie ze stosów kart rzuconych na "stó³", a¿ do opró¿nienia stosów
		//najpierw karta przegranego
		while (*pulaGracz1 != NULL && *pulaGracz2 != NULL) {
			pop(pulaGracz1, temp2);
			push(pulaWspolna, temp2->figura, temp2->kolor, temp2->wartosc);
			pop(pulaGracz2, temp1);
			push(pulaWspolna, temp1->figura, temp1->kolor, temp1->wartosc);
		}
		//przenies z puliWspolnej na stos zwyciêzcy
		while (*pulaWspolna != NULL) {
			pop(pulaWspolna, temp1);
			push(Gracz2, temp1->figura, temp1->kolor, temp1->wartosc);
		}
	}
}

//wykorzystanie biblioteki window.h do zmiany koloru tekstu wyœwietlanego
void ktoWygrywaRozdanie(int zwyciezca) {
	HANDLE kolor;
	kolor = GetStdHandle(STD_OUTPUT_HANDLE); 
	if (zwyciezca == 1) {
		SetConsoleTextAttribute(kolor, 12); //red
		printf("\tGRACZ%d WYGRYWA ROZDANIE!\n\n", zwyciezca);
	}
	else if (zwyciezca == 2) {
		SetConsoleTextAttribute(kolor, 9); //blue
		printf("\tGRACZ%d WYGRYWA ROZDANIE!\n\n", zwyciezca);
	}
	else {}
	SetConsoleTextAttribute(kolor, 7); //default
	}

//funckja zwraca 1, gdy któryœ z Graczy nie ma kart; w przeciwnym wypadku zwraca 0
int czyNULL(struct talia *Gracz1, struct talia *Gracz2) {
	if (Gracz1 == NULL || Gracz2 == NULL)
		return 1;
	else
		return 0;
}

int wybierzZwyciezce(struct talia *Gracz1, struct talia *Gracz2) {
	int zwyciezca = 0;
	if (Gracz1 != NULL && Gracz2 == NULL)
		return 1;
	else
		return 2;
}

//u¿ytkownik wybiera czas opoŸnienia
int czyOpozniac() {
	char znak;
	system("cls");
	printf("Wybierz opoŸnienie wyœwietlania:\n1.\t2 s\n2.\t1 s\n3.\t500 ms\n4.\t250 ms\nInny przecisk, by nie ustawiæ opóŸnieñ\n\nNaciœniej spacjê w ka¿dym momencie rozgrywki, ¿eby zmieniæ opóŸnienie na 0\n");
	znak = _getch();
	system("cls");
	if (znak == '1')
		return 2000;
	else if (znak == '2')
		return 1000;
	else if (znak == '3')
		return 500;
	else if (znak == '4')
		return 250;
	else if (znak == '5')
		return 25;
	else
		return 0;
}

//wyœwietla menu powitalne, uniemo¿liwia wpisanie innych znaków ni¿ '0' i '1', zwraca jeden z tych znaków
char menuPowitalne() {
	char wybor=0;
	printf("Symulator wojny \"Stalingrad 2017\"\nWybierz tryb gry:\n");
	do {
		printf("1.\tRozpocznij grê!\n");
		printf("0.\tWyjdz\n");
		wybor=_getch();
		system("cls");
		printf("Wybierz 1 lub 0!\n");
	} while (wybor != '0' && wybor != '1');
	system("cls");
	return wybor;
}

//zwraca koñcówkê odmiany wyrazu "runda" w zale¿noœci od iloœci rund
char* odmianaRunda(int liczbaRund) {
	if (liczbaRund == 1) 
		return "ê";	
	else if ((liczbaRund % 10 == 2 && liczbaRund %100 !=12)   || (liczbaRund % 10 == 3 && liczbaRund % 100 != 13) || (liczbaRund % 10 == 4 && liczbaRund % 100 != 14))
		return "y";
	else
		return "";
}

//wyœwietla komunikat koñcowy, z informacj¹ o liczbie rund, zeruje liczbê rund i zwraca tê wartoœæ
int komunikatKoncowy(int wynik, int i ) {
	printf("GRACZ%d utraci³ wszystkie karty!\n\n",wynik%2+1);
	printf("-------------------------------------------------------------------");
	printf("\nGRACZ%d wykaza³ siê wiêkszym sprytem i odniós³ mia¿d¿¹ce zwyciêztwo!\n\t\tGratulacje GRACZU%d, dobra robota!\n",wynik,wynik);
	printf("-------------------------------------------------------------------\n\n");
	printf("Ta rozgrywka trwa³a %d rund%s ", i, odmianaRunda(i));
	wait(1000);
	printf("za du¿o.\n");
	wait(1000);
	return i = 0;
}

//uniemo¿liwia wpisanie innego znaku ni¿ '1' i '0' i zwraca znak jeden z dwóch tych znaków
char warunekWyjscia() {
	char wybor;
	do {
		printf("\nChcesz zagraæ jeszcze raz?\n1.\tDlaczego mia³bym nie chcieæ?\n0.\tWyjdŸ.\n");
		wybor = _getch();
		system("cls");
		printf("Wybierz 1 lub 0!\n");
	} while (wybor != '0' && wybor != '1');
	return wybor;
}

//usuwa elementy ze stosów
void oproznijStosy(struct talia **Gracz1, struct talia **Gracz2, struct karta *card) {
	while (*Gracz1 != NULL)
		pop(Gracz1, card);
	while (*Gracz2 != NULL)
		pop(Gracz2, card);	
}

//zwalnia pamiêæ wszystkich stosów w programie
void zwolnijPamiecStosy(struct talia **Gracz1, struct talia **Gracz2, struct talia **StosGracz1, struct talia **StosGracz2, struct talia **Pula) {
	free(*Gracz1);
	free(*Gracz2);
	free(*Pula);
	free(*StosGracz1);
	free(*StosGracz2);
}

int main() {
	struct talia *Gracz1 = NULL;		//stos kart Gracza1
	struct talia *Gracz2 = NULL;		//stos kart Gracza2
	struct talia *StosGracz1 = NULL;	//stos, na który rzucane s¹ karty Gracza1 w trakcie rundy
	struct talia *StosGracz2 = NULL;	//stos, na który rzucane s¹ karty Gracza2 w trakcie rundy
	struct talia *Pula = NULL;			//stos na pule kart, które zostan¹ przeniesione na stos zwyciêzcy - Gracz1 lub Gracz2
	struct karta wartosci1, wartosci2;	//zmienne strukturalne do przechowywania wartosci rand w trakcie rundy
	struct karta temp1, temp2;			//tymcasowe zmienne strukkturalne do przechpwywania wartosci karty przy funkcjach push, pop
	char ***tab;			//tablica na wczytane stringi z .csv
	int wynikRundy=0;		//zmienna na wynik rundy: 1-wygrywa Gracz1, 2-wygrywa Gracz2, 0-wojna
	int wygrany = 0;		//zmienna przyjmuj¹ca wart 1, gdy wygrywa gracz1, 2, gdy wygrywa gracz2
	int i = 0;				//licznik rund
	char wybor;				//zmienna g³ównego menu; umo¿liwia rozpoczêcie rozgrywki lub wyjœæie z programu
	int opoznienie;			//zmienna na wartoœæ opóŸnienia zwracanego przez  funkcjê czyOpozniac()
	setlocale(LC_ALL, "polish_poland");
		
	wybor=menuPowitalne();
	while(wybor !='0'){
		tab = alokujDwuwymiarowaChar(TALIA,KARTA,ZNAKI);		//alokacja tablicy dwuwymiarowej char
		tab = wczytajTalie();				//wczytuje dane z pliku i umieszcza w tablicy
		tasujRozdaj(&Gracz1, &Gracz2, tab);
		zwolnijTablicaDwuwymiarowaChar(tab, TALIA, KARTA);
		wyswietlStos(Gracz1);
		wyswietlStos(Gracz2);
		opoznienie = czyOpozniac();
		printf("START!\n\n");
		do {
			if (_kbhit() && _getch() == ' ') //czeka na wciœniêcie spacji
				opoznienie = 0;
			do {
				wyswietlRunde(Gracz1, Gracz2);
				runda(&i, &Gracz1, &Gracz2, &StosGracz1, &StosGracz2, &wartosci1, &wartosci2);
				wynikRundy = stanRozgrywki(wartosci1, wartosci2);	//przyjmuje 1,2 lub 0
				if (wynikRundy == 0) {	//wojna
					if (czyNULL(Gracz1, Gracz2) == 1)	//jeœli któryœ z graczy nie ma kart na stosie, wychodzi z pêtli
						break;
					runda(&i, &Gracz1, &Gracz2, &StosGracz1, &StosGracz2, &temp1, &temp2);
					printf("\nWOJNA! Gracze rzucili odwrócone karty (%s %s i %s %s)\n\n", temp1.figura, temp1.kolor, temp2.figura, temp2.kolor);
					wait(opoznienie);
					if (czyNULL(Gracz1, Gracz2) == 1)	//jeœli któryœ z graczy nie ma kart na stosie, wychodzi z pêtli
						break;
				}
			} while (wynikRundy == 0);	//pêtla twa, dopóki wojna siê nie skoñczy
			ktoWygrywaRozdanie(wynikRundy);	//wyœwietla komunikat, kto wygra³ rundê
			doWspolnejPuli_naStosWygranego(&Gracz1, &Gracz2, &StosGracz1, &StosGracz2, &Pula, &temp1, &temp2, wynikRundy); //przerzuca karty na stos wygranego
			wait(opoznienie);
		} while (Gracz1 != NULL && Gracz2 != NULL);
		wygrany = wybierzZwyciezce(Gracz1, Gracz2);		//przyjmuje 1 lub 2, ostateczny wynik
		i=komunikatKoncowy(wygrany, i);					//zwraca i=0, zerujê counter rund, wyœwietla komunikat, kto wygra³
		wybor = warunekWyjscia();						
		oproznijStosy(&Gracz1, &Gracz2, &temp1);	//opróŸnia stosy Graczy
	} 
	zwolnijPamiecStosy(&Gracz1, &Gracz2, &StosGracz1, &StosGracz2, &Pula);
	return 0;
}