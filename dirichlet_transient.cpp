#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

using std::string;

string filename("data.csv");

const int n = 20;
string buffer[n];

FILE *fp;

using namespace std;

typedef long double Real;

struct TVec {
    int dim;
    Real coor[n];
};

void dirichlet(TVec &Tp, TVec &T, Real T0, Real TL, Real dx, Real dt) {
    T.coor[0] = T0;
    for (int i = 1; i <= n; i++) {
        T.coor[i] = Tp.coor[i + 1] * dt / pow(dx, 2) +
                    (1 - 2 * dt / pow(dx, 2)) * Tp.coor[i] +
                    dt / pow(dx, 2) * Tp.coor[i - 1];
    }
    T.coor[n - 1] = TL;

    for (int i = 0; i < n; i++) {
        Tp.coor[i] = T.coor[i];
    }
}

int main() {
    TVec X, T, Tp;

    Real L = 1;

    Real T0 = 493.15;
    Real TL = 373.15;

    Real dx = L / (n - 1);
    Real dt = 0.8 * pow(dx, 2) / 2;

    X.coor[0] = 0;
    Tp.coor[0] = T0;

    for (int i = 1; i < n; i++) {
        X.coor[i] = X.coor[i - 1] + dx;
        Tp.coor[i] = T0;
    }

    for (int i = 0; i < n; i++) {
        buffer[i] = to_string(X.coor[i]);
        buffer[i] += ",";
    }

    Tp.coor[n - 1] = TL;

    int col = 1;

    fp = popen("gnuplot -persist", "w");

    for (int k = 0; k < 1001; k++) {
        dirichlet(Tp, T, T0, TL, dx, dt);

        fprintf(fp, "plot '-' using 1:2 with lines\n");
        for (int i = 0; i < n; i++) {
            fprintf(fp, "%Lf %Lf\n", X.coor[i], T.coor[i]);
        }
        fprintf(fp, "e\n");
        fflush(fp);

        usleep(1e5);
    }

    pclose(fp);
}