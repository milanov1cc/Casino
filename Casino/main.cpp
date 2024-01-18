#include <iostream>
#include <cstring>
#include <algorithm>
#include <ctime>

using namespace std;

class NedozvoljenaSituacija : public exception {
public:
    const char* what() const throw() {
        return "Nedozvoljena situacija!";
    }
};

class Igrac {
private:
    static int nextID;
    int id;
    char ime[81];
    double balans;

public:
    Igrac(const char* ime, double balans) : id(nextID++), balans(balans) {
        strncpy_s(this->ime, ime, 80);
        this->ime[80] = '\0';
    }

    Igrac(const Igrac& other) : id(other.id), balans(other.balans) {
        strncpy_s(this->ime, other.ime, 80);
        this->ime[80] = '\0';
    }

    int getID() const { return id; }
    const char* getIme() const { return ime; }
    double getBalans() const { return balans; }
    void setBalans(double balans) { this->balans = balans; }

    friend istream& operator>>(istream& in, Igrac& igrac) {
        in >> igrac.ime >> igrac.balans;
        return in;
    }

    friend ostream& operator<<(ostream& out, const Igrac& igrac) {
        out << "ID: " << igrac.id << ", Ime: " << igrac.ime << ", Balans: " << igrac.balans;
        return out;
    }
};

int Igrac::nextID = 1;

class Skor {
private:
    double rezultat;
    Igrac igrac;

public:
    Skor() : rezultat(0), igrac("", 0) {}
    Skor(const Igrac& igrac, double rezultat = 0.0) : igrac(igrac), rezultat(rezultat) {}

    double getRezultat() const { return rezultat; }
    const Igrac& getIgrac() const { return igrac; }

    void setRezultat(double rezultat) { this->rezultat = rezultat; }
    void setIgrac(const Igrac& igrac) { this->igrac = igrac; }

    //Skor& operator=(const Skor&) = delete;

    friend ostream& operator<<(ostream& out, const Skor& skor) {
        out << "Igrac: " << skor.igrac << ", Rezultat: " << skor.rezultat;
        return out;
    }
};

class Igra {
protected:
    char naziv[31];
    Skor* skorovi;
    int brojSkorova;
    int kapacitet;

public:
    Igra(const char* naziv) : brojSkorova(0), kapacitet(10) {
        strncpy_s(this->naziv, naziv, 30);
        this->naziv[30] = '\0';
        skorovi = new Skor[kapacitet];
    }

    ~Igra() {
        delete[] skorovi;
    }

    const char* getNaziv() const { return naziv; }
    const Skor* getSkorovi() const { return skorovi; }

    void dodajSkor(const Skor& skor) {
        if (brojSkorova == kapacitet) {
            kapacitet *= 2;
            Skor* noviSkorovi = new Skor[kapacitet];
            for (int i = 0; i < brojSkorova; i++) {
                noviSkorovi[i] = skorovi[i];
            }
            delete[] skorovi;
            skorovi = noviSkorovi;
        }
        skorovi[brojSkorova++] = skor;
    }

    Skor vratiSkor(double vrednost) const {
        for (int i = 0; i < brojSkorova; i++) {
            if (skorovi[i].getRezultat() == vrednost) {
                return skorovi[i];
            }
        }
        return Skor(Igrac("", 0), 0);
    }

    void sortirajSkorove(bool opadajuce) {
        sort(skorovi, skorovi + brojSkorova, [opadajuce](const auto& a, const auto& b) {
            return opadajuce ? a.getRezultat() > b.getRezultat() : a.getRezultat() < b.getRezultat();
        });
    }


    virtual void zapocniIgru() = 0;
    virtual void promeniBalansIgraca(int id, double iznos, double budzet) = 0;

    friend ostream& operator<<(ostream& out, const Igra& igra) {
        out << "Naziv igre: " << igra.naziv << "\n";
        out << "Skorovi:\n";
        for (int i = 0; i < igra.brojSkorova; i++) {
            out << igra.skorovi[i] << "\n";
        }
        return out;
    }
};

class Pseudorulet : public Igra {
private:
    int krajnjiIshod;
    bool polja[36];
    char boja;
    bool parniBrojevi;

public:
    Pseudorulet() : Igra("Pseudorulet"), krajnjiIshod(0), boja('r'), parniBrojevi(false) {
        for (int i = 0; i < 36; i++) {
            polja[i] = false;
        }
    }

    void novaIgra() {
        krajnjiIshod = 0;
        boja = 'r';
        parniBrojevi = false;
        for (int i = 0; i < 36; i++) {
            polja[i] = false;
        }
    }

    void generisiBroj() {
        srand(time(0));
        krajnjiIshod = rand() % 37;
    }

    void odrediParnost() {
        parniBrojevi = krajnjiIshod % 2 == 0;
    }

    void odrediBoju() {
        if ((krajnjiIshod >= 1 && krajnjiIshod <= 10) || (krajnjiIshod >= 19 && krajnjiIshod <= 28)) {
            boja = krajnjiIshod % 2 == 0 ? 'c' : 'r';
        }
        else if ((krajnjiIshod >= 11 && krajnjiIshod <= 18) || (krajnjiIshod >= 29 && krajnjiIshod <= 36)) {
            boja = krajnjiIshod % 2 == 0 ? 'r' : 'c';
        }
    }

    void postaviBroj(int broj) {
        if (broj >= 0 && broj < 36) {
            polja[broj] = true;
        }
    }

    void zapocniIgru() override {
        novaIgra();
        generisiBroj();
        odrediParnost();
        odrediBoju();
    }

    void promeniBalansIgraca(int id, double iznos, double budzet) override {
        for (int i = 0; i < brojSkorova; i++) {
            if (skorovi[i].getIgrac().getID() == id && budzet >= iznos) {
                Igrac igrac = skorovi[i].getIgrac();
                igrac.setBalans(igrac.getBalans() + iznos);
                skorovi[i].setIgrac(igrac);
                break;
            }
        }
    }

    bool& operator[](size_t index) {
        return polja[index];
    }

    void operator*=(char novaBoja) {
        if (novaBoja == 'r' || novaBoja == 'c') {
            boja = novaBoja;
        }
    }

    void operator/=(bool noviParniBrojevi) {
        parniBrojevi = noviParniBrojevi;
    }

    void operator-=(bool noviParniBrojevi) {
        parniBrojevi = !noviParniBrojevi;
    }

    void operator+=(Igrac& igrac) {
        double iznos = polja[krajnjiIshod] ? 1 : -1;
        igrac.setBalans(igrac.getBalans() + iznos);
    }
};

class Blekdzek : public Igra {
private:
    int spil[52];
    int ruka[9];

public:
    Blekdzek() : Igra("Blekdzek") {
        srand(time(0));
        for (int i = 0; i < 52; i++) {
            spil[i] = i % 13 + 1;
        }
        for (int i = 0; i < 9; i++) {
            ruka[i] = 0;
        }
    }

    void novaIgra() {
        for (int i = 0; i < 52; i++) {
            spil[i] = i % 13 + 1;
        }
        for (int i = 0; i < 9; i++) {
            ruka[i] = 0;
        }
    }

    int generisiBroj() {
        return rand() % 13 + 1;
    }

    int izracunajRezultat() {
        int rezultat = 0;
        for (int i = 0; i < 9; i++) {
            if (ruka[i] >= 2 && ruka[i] <= 10) {
                rezultat += ruka[i];
            }
            else if (ruka[i] >= 11 && ruka[i] <= 13) {
                rezultat += 1;
            }
            else if (ruka[i] == 1) {
                rezultat += (rezultat > 10) ? 1 : 11;
            }
        }
        return rezultat;
    }

    int izvuciKartu() {
        int index = rand() % 52;
        while (spil[index] == 0) {
            index = rand() % 52;
        }
        int karta = spil[index];
        spil[index] = 0;
        for (int i = 0; i < 9; i++) {
            if (ruka[i] == 0) {
                ruka[i] = karta;
                break;
            }
        }
        return izracunajRezultat();
    }

    void zapocniIgru() override {
        novaIgra();
        while (izvuciKartu() <= 21);
    }

    void promeniBalansIgraca(int id, double iznos, double budzet) override {
        for (int i = 0; i < brojSkorova; i++) {
            if (skorovi[i].getIgrac().getID() == id && budzet >= iznos) {
                Igrac igrac = skorovi[i].getIgrac();
                igrac.setBalans(igrac.getBalans() + iznos);
                skorovi[i].setIgrac(igrac);
                break;
            }
        }
    }
};

class Kazino {
private:
    Igra* igre[10];
    int brojIgara;
    double budzet;

public:
    Kazino(double budzet) : budzet(budzet), brojIgara(0) {}

    double getBudzet() const { return budzet; }

    void dodajIgru(Igra* igra) {
        if (brojIgara < 10) {
            igre[brojIgara++] = igra;
        }
        else {
            throw NedozvoljenaSituacija();
        }
    }

    void prikaziIgre() const {
        for (int i = 0; i < brojIgara; i++) {
            cout << "Igra " << i + 1 << ": " << igre[i]->getNaziv() << "\n";
        }
    }

    void povecajBalansIgraca(Igrac& igrac, double iznos) {
        if (budzet >= iznos) {
            igrac.setBalans(igrac.getBalans() + iznos);
            budzet -= iznos;
        }
        else {
            throw NedozvoljenaSituacija();
        }
    }

    void odaberiIgru(int index, Igrac& igrac) {
        if (index >= 0 && index < brojIgara && budzet > 500 && igrac.getBalans() > 10) {
            igre[index]->zapocniIgru();
        }
        else {
            throw NedozvoljenaSituacija();
        }
    }
};

int main() {
    Kazino kazino(1000);
    Igrac igrac("Igrac", 100);
    Pseudorulet pseudorulet;
    Blekdzek blekdzek;

    kazino.dodajIgru(&pseudorulet);
    kazino.dodajIgru(&blekdzek);

    int izbor;
    do {
        cout << "1. Prikazi igre\n";
        cout << "2. Odaberi igru\n";
        cout << "3. Izlaz\n";
        cout << "Izbor: ";
        cin >> izbor;

        try {
            switch (izbor) {
            case 1:
                kazino.prikaziIgre();
                break;
            case 2:
                int index;
                cout << "Unesite indeks igre: ";
                cin >> index;
                kazino.odaberiIgru(index, igrac);
                break;
            case 3:
                cout << "Izlaz iz programa.\n";
                break;
            default:
                cout << "Nepostojeca opcija!\n";
                break;
            }
        }
        catch (NedozvoljenaSituacija& e) {
            cout << e.what() << "\n";
        }
    } while (izbor != 3);

    return 0;
}