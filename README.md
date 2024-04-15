# Aplicatie gestionare inventar supermarket

## Tema proiectului

Realizati o aplicatie care sa gestioneze activitatile desfasurate intr-un supermarket.  
Articolele din fisierul binar organizat secvential au urmatoarea structura:

| Produs                |
| --------------------- |
| Nume                  |
| Categorie             |
| Cod de bare           |
| Pret                  |
| Cantitate disponibila |
| Data de expirare      |

## Detalii tehnice

Codul proiectului este impartit in 2 directoare:

1. `include/`: contine fisierele header **.h**
2. `src/`: contine fisierele sursa **.c**

In fisierele header se afla declaratiile atat ale structurilor, cat si ale functiilor folosite,  
cele din urma fiind insotite de documentatie privind scopul functiei, parametrii primiti si datele returnate.

### Continutul fisierelor

- `database.h`/`database.c`: Aici rezida principalul cod in jurul caruia este realizat proiectul. Fisierele contin declaratiile  
  necesare si implementarea unei baze de date sub forma unui fisier binar, avand intrarile organizate secvential.  
  Baza de date este proiectata in asa maniera incat sa fie **_type agnostic_**, adica este o baza de date generica  
  ce poate lucra cu orice tip de date, atat timp cat toate intrarile sunt de acelasi fel(au aceeasi marime). Acest  
  lucru este posibil prin folosirea functiilor primite ca parametrii pentru realizarea diferitelor operatii, precum:  
  actualizarea unei intrari, afisarea unei intrari si verificarea potrivirii a doua intrari. Astfel, cel care utilizeaza  
  baza de date este responsabil sa ofere functiile si datele necesare, asa cum sunt documentate in `database.h`, pentru  
  functionarea corecta a codului.
  Operatiile de actualizare, stergere si generare a unui raport au complexitate liniara, intrucat fisierul binar trebuie iterat  
  secvential, iar in cazul in care se cauta intrari specifice, fiecare articol trebuie comparat cu un anumit criteriu pentru a determina  
  daca face parte din multimea intrarilor cautate. Operatia de adaugare a unui element, facand abstractie de potentiala complexitate a  
  functiei _fseek()_, are complexitate constanta, doar scriindu-se informatiile articolului la finalul fisierului.

- `store_manager.h`/`store_manager.c`: Aici se afla declaratia structurii unui produs din baza de date, dar si declaratiile si  
  implementarile functiilor ajutatoare gandite pentru a interactiona cu baza de date, precum: functii care verifica daca doua intrari se potrivesc  
  in functie de un criteriu(cod de bare, nume, categorie), functii ce actualizeaza diferite campuri din structura produsului si functia  
  de afisare a unui produs(in cadrul unui raport).

- `cli.h`/`cli.c`: Aici se afla implementarea programului din cli, al meniului, cu care interactioneaza utilizatorul atunci cand ruleaza programul.
  Meniul are urmatoarea structura:

```
1. Creaza baza de date(fisierul binar)
2. Incarca o baza de date existenta(fisierul binar)
3. Adauga un produs
4. Actualizeaza un produs unic
\_
  \_ 4.1. Actualizeaza pret
  \_ 4.2. Actualizeaza cantitate
  \_ 4.3. Actualizeaza data de expirare
5. Aplica discount unei categorii de produse
6. Sterge un produs unic
7. Genereaza un raport total(fisier text)
8. Genereaza un raport pentru o categorie(fisier text)
9. Gaseste un produs dupa nume(afisare pe ecran)
10. Iesire
```

- `error.h`: Aici se afla **_enum status_** folosit de functiile din `cli.c` ce returneaza statusul operatiei, si macro-ul **_DIE_** folosit,  
  in mare parte, pentru a verifica daca alocarile de memorie au avut loc cu succes si, in caz contrar, sa opreasca executia programului.
