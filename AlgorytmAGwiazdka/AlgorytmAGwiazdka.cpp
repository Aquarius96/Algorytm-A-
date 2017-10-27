#include <iostream>
#include <fstream>
#include <cmath>
#include<vector>
#include "ConsoleColor.h"

using namespace std;

class Point {

public:
	bool odwiedzony;
	bool policzone;
	int x, y, xParent, yParent;
	double wartosc;
	Point(int, int);
	void setParent(int x, int y)
	{
		xParent = x;
		yParent = y;
	}
};

Point::Point(int a, int b)
{
	x = a;
	y = b;
	policzone = false;
	odwiedzony = false;
}

Point start(19, 0);
Point meta(6, 1);
vector<Point> otwarta;
vector<Point> zamknieta;
Point current(0, 0);

int koszt(Point aktualny)
{
	int licznik=0;
	int x = aktualny.xParent;
	int y = aktualny.yParent;
	while (x != start.x || y != start.y)
	{
		if (zamknieta.size() > 1)
		{
			licznik++;
			 for (int i = 0; i < zamknieta.size(); i++)
			 {
				 if (zamknieta[i].x == x && zamknieta[i].y == y)
				 {
					 x = zamknieta[i].xParent;
					 y = zamknieta[i].yParent;
				 }
			 }
		}
		else return 1;	
	}
	return licznik;
}

double heurystyka(Point aktualny)
{
	return sqrt(pow(double(aktualny.x - start.x), 2.0) + pow(double(aktualny.y - start.y), 2.0));
}

double ObliczWartosc(Point aktualny)
{
	return koszt(aktualny) + heurystyka(aktualny);
}

bool SprawdzListe(Point obecny, vector<Point> lista)
{
	bool wynik;
	for (int i = 0; i<lista.size(); i++)
	{
		if ((lista[i].x == obecny.x && lista[i].y == obecny.y) || (start.x == obecny.x && start.y == obecny.y))
		{
			return false;
		}
		else
		{
			wynik = true;
		}
	}
	return wynik;
}

void Wstaw(Point obecny,bool wynik)
{
	if (wynik)
	{
		if (obecny.policzone)
		{
			if (obecny.wartosc>ObliczWartosc(obecny))
			{
				obecny.setParent(current.x, current.y);
				obecny.wartosc = ObliczWartosc(obecny);
			}
		}
		else
		{
			obecny.setParent(current.x, current.y);
			obecny.wartosc = ObliczWartosc(obecny);
		}
		otwarta.push_back(obecny);
	}
}

void WstawBezPowtarzania(Point obecny) 
{
	if (otwarta.size()>0)
	{
		bool wynik = SprawdzListe(obecny,otwarta) && SprawdzListe(obecny,zamknieta);
		Wstaw(obecny, wynik);
	}
	else
	{
		bool wynik = SprawdzListe(obecny,zamknieta);
		Wstaw(obecny, wynik);
	}
}

void RozejrzyjSie(Point pkt, double **G)
{
	int x = pkt.x;
	int y = pkt.y;
	if (y>0 && G[x][y - 1] != 5 && !pkt.odwiedzony)
	{
		Point obecny(x, y - 1);
		WstawBezPowtarzania(obecny);
	}
	if (x>0 && G[x - 1][y] != 5 && !pkt.odwiedzony)
	{
		Point obecny(x - 1, y);
		WstawBezPowtarzania(obecny);
	}
	if (y<19 && G[x][y + 1] != 5 && !pkt.odwiedzony)
	{
		Point obecny(x, y + 1);
		WstawBezPowtarzania(obecny);
	}
	if (x<19 && G[x + 1][y] != 5 && !pkt.odwiedzony)
	{
		Point obecny(x + 1, y);
		WstawBezPowtarzania(obecny);
	}
}

void WybierzPunkt()
{
	double wynik = otwarta[0].wartosc;
	for (int i = 0; i<otwarta.size(); i++)
	{
		if (otwarta[i].wartosc<wynik)
		{
			wynik = otwarta[i].wartosc;
		}
	}
	for (int i = 0; i<otwarta.size(); i++)
	{
		if (otwarta[i].wartosc == wynik)
		{
			current.x=otwarta[i].x;
			current.y=otwarta[i].y;
			zamknieta.push_back(otwarta[i]);
			otwarta.erase(otwarta.begin() + i);
			break;
		}
	}
}

void Wroc(double **G)
{
	int x = current.x;
	int y = current.y;
	while (x != start.x || y != start.y)
	{
		for (int i = 0; i<zamknieta.size(); i++)
		{
			if (x == zamknieta[i].x && y == zamknieta[i].y)
			{
				G[x][y] = 3;
				x = zamknieta[i].xParent;
				y = zamknieta[i].yParent;
				break;
			}
		}
		G[x][y] = 3;
	}
}

void ObliczTrase(double **G)
{
	current.x=start.x;
	current.y=start.y;
	zamknieta.push_back(start);
	RozejrzyjSie(start, G);
	while ((zamknieta.back().x != meta.x || zamknieta.back().y != meta.y) && otwarta.size()>0)
	{
		WybierzPunkt();
		RozejrzyjSie(current, G);
	}

	if (zamknieta.back().x == meta.x && zamknieta.back().y == meta.y)
	{
		Wroc(G);
	}
	else if (otwarta.size() == 0)
	{
		cout << "niestety nie udalo sie dotrzec do celu";
	}
}

void WypiszMape(double **G)
{
	for (int i = 0; i<20; i++)
	{
		for (int j = 0; j<20; j++)
		{
			if (G[i][j] == 5)
			{
				cout << " " << red << G[i][j] << white;
			}
			else if (G[i][j] == 3)
			{
				cout << " " << green << G[i][j] << white;
			}
			else
			{
				cout << " " << G[i][j];
			}
		}cout << "\n";
	}
}

double** ZrobMape(int wym1, int wym2, string fileName) 
{
	int rows = wym2;
	double **G;
	G = new double*[rows];
	while (rows--) G[rows] = new double[wym1];
	ifstream plik(fileName.c_str());
	for (unsigned int i = 0; i<wym2; i++)
	{
		for (unsigned int j = 0; j<wym1; j++)
		{
			plik >> G[i][j];
		}
	}
	plik.close();
	return G;
}

int main(void) {
	string plik = "grid.txt";
	int wym2 = 20;
	int wym1 = 20;
	double **Grid = ZrobMape(wym1, wym2, plik);
	ObliczTrase(Grid);
	cout << "start " << start.x << " " << start.y << "\n";
	cout << "meta  " << meta.x << " " << meta.y << "\n";
	WypiszMape(Grid);
	getchar();
	return 0;
}



