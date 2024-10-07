#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;


using namespace std;

// Funkcja do obliczania całkowitego czasu zakończenia
int oblicz_calkowity_czas_zakonczenia(const vector<vector<int>>& czasy_przetwarzania, const vector<int>& kolejnosc) {
    int liczba_zadan = kolejnosc.size();
    int liczba_maszyn = czasy_przetwarzania[0].size();

    vector<int> czas_zakonczenia_przetwarzania_na_maszynie(liczba_maszyn, 0);
    vector<int> czas_zakonczenia_przetwarzania_zadania_na_ostatniej_maszynie(liczba_zadan, 0);

    for (int i = 0; i < liczba_zadan; ++i) {
        int indeks_zadania = kolejnosc[i];
        czas_zakonczenia_przetwarzania_zadania_na_ostatniej_maszynie[i] = czas_zakonczenia_przetwarzania_na_maszynie[0] + czasy_przetwarzania[indeks_zadania][0];
        czas_zakonczenia_przetwarzania_na_maszynie[0] = czas_zakonczenia_przetwarzania_zadania_na_ostatniej_maszynie[i];

        for (int j = 1; j < liczba_maszyn; ++j) {
            czas_zakonczenia_przetwarzania_zadania_na_ostatniej_maszynie[i] = max(czas_zakonczenia_przetwarzania_zadania_na_ostatniej_maszynie[i], czas_zakonczenia_przetwarzania_na_maszynie[j]) + czasy_przetwarzania[indeks_zadania][j];
            czas_zakonczenia_przetwarzania_na_maszynie[j] = czas_zakonczenia_przetwarzania_zadania_na_ostatniej_maszynie[i];
        }
    }

    return czas_zakonczenia_przetwarzania_zadania_na_ostatniej_maszynie[liczba_zadan - 1];
}

// Funkcja generująca losowe sąsiednie rozwiązanie przez zamianę dwóch zadań
vector<int> generuj_sasiednie_rozwiazanie(const vector<int>& obecne_rozwiazanie) {
    vector<int> nowe_rozwiazanie = obecne_rozwiazanie;
    int n = nowe_rozwiazanie.size();
    int i = rand() % n;
    int j;
    
    do {
    j = rand() % n;
    } while (i == j);

    swap(nowe_rozwiazanie[i], nowe_rozwiazanie[j]);
    return nowe_rozwiazanie;
}

// Funkcja symulowanego wyżarzania
vector<int> algorytm_symulowanego_wyzarzania(const vector<vector<int>>& czasy_przetwarzania, vector<int> kolejnosc_startowa, double temperatura_poczatkowa, double wspolczynnik_chlodzenia, int liczba_iteracji) {
    vector<int> aktualne_rozwiazanie = kolejnosc_startowa;
    int aktualny_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, aktualne_rozwiazanie);
    vector<int> najlepsze_rozwiazanie = aktualne_rozwiazanie;
    int najlepszy_czas = aktualny_czas;
    double temperatura = temperatura_poczatkowa;

    for (int iteracja = 0; iteracja < liczba_iteracji; iteracja++) {
        vector<int> nowe_rozwiazanie = generuj_sasiednie_rozwiazanie(aktualne_rozwiazanie);
        int nowy_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, nowe_rozwiazanie);

        if (nowy_czas < najlepszy_czas) {
            najlepsze_rozwiazanie = nowe_rozwiazanie;
            najlepszy_czas = nowy_czas;
        }

        if (nowy_czas < aktualny_czas || exp((aktualny_czas - nowy_czas) / temperatura) > ((double) rand() / RAND_MAX)) {
            aktualne_rozwiazanie = nowe_rozwiazanie;
            aktualny_czas = nowy_czas;
        }

        temperatura *= wspolczynnik_chlodzenia;
    }

    return najlepsze_rozwiazanie;
}

int main() {
    srand(time(0));

    httplib::Server svr;

    svr.Post("/optimize", [](const httplib::Request& req, httplib::Response& res) {
        json input = json::parse(req.body);

        int liczba_zadan = input["liczba_zadan"];
        int liczba_maszyn = input["liczba_maszyn"];
        vector<vector<int>> czasy_przetwarzania = input["czasy_przetwarzania"];
        double temperatura_poczatkowa = input["temperatura_poczatkowa"];
        double wspolczynnik_chlodzenia = input["wspolczynnik_chlodzenia"];
        int liczba_iteracji = input["liczba_iteracji"];

        vector<int> kolejnosc_startowa(liczba_zadan);
        for (int i = 0; i < liczba_zadan; ++i) {
            kolejnosc_startowa[i] = i;
        }

        vector<int> najlepsze_rozwiazanie = algorytm_symulowanego_wyzarzania(czasy_przetwarzania, kolejnosc_startowa, temperatura_poczatkowa, wspolczynnik_chlodzenia, liczba_iteracji);

        int najlepszy_czas = oblicz_calkowity_czas_zakonczenia(czasy_przetwarzania, najlepsze_rozwiazanie);

        json result;
        result["najlepsze_rozwiazanie"] = najlepsze_rozwiazanie;
        result["najlepszy_czas"] = najlepszy_czas;

        res.set_content(result.dump(), "application/json");
    });

    cout << "Server starting on port 8080..." << endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}