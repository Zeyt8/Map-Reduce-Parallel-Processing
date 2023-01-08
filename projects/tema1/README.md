# Procesare paralela folosind paradigma Map-Reduce

In main citesc din fisierul principal lista de fisiere de input si le adaug la o coada. De asemenea initializez orice structura de sincronziare ce va fi folosita.

Se construiesc argumentele pentru mapper si reduceri si pornesc threadurile.

Puterile perfecte se stocheaza intr-o matrice[nrMapper][nrReduceri] de seturi neordonate. Prima dimensiune a matricii se foloseste pentru a atribui fiecarui mapper un array de seturi, adica fiecare mapper are un rand din matrice. In acest array, indexul reprezinta puterea pentru care se calculeaza daca numarul este putere perfecta. Setul contine numerele ce sunt putere perfecta.

## Mapper arg

* list - lista de fisiere de input
* powerLists - array-ul cu seturi de puteri perfecte pentru acest mapper
* maxPower - puterea maxima pana la care se calculeaza daca e putere perfecta
* barrier - o bariera
* mutex - un mutex

## Reducer arg

* id - puterea pentru care scrie acest reducer
* powerLists - matricea de seturi de puteri perfecte
* mappers - numarul de mapperi
* reducers - numarul de reduceri
* barrier - o bariera

## Mapper

Mapperul se uita in coada de fisiere si daca exista extrage unul, daca nu, se iese din while. Extragerea din coada se incapsuleaza intr-un mutex.

Se incepe citirea fisierului. Daca numarul este 1, se adauga ca si putere perfecta pentru toate puterile. Daca este diferit se calculeaza toate puterile pentru care numarul este putere perfecta. Numarul se adauga in setul corespunzatoare.

Pentru fiecare fisier se creeaza un set nou, care la final se adauga in setul pentru acest mapper. Am observat ca acest lucru imbunatateste performanta.

Mapperul asteapta la bariera de la final toti ceilalti mapperi.

### Calcul putere perfecta

Se trece cu un for prin toate puterile de la 2 la maxPower.

Pentru fiecare putere se face o cautare binara pentru a gasi un numar A in asa fel incat A<sup>exp</sup> sa fie egal cu numarul.

## Reducer

Reducerii au la inceput o bariera care coincide cu cea de la mapperi. In acest mod ei vor porni doar dupa ce toti mapperi termina treaba.

Fiecare reducer are un id cu puterea pe care o prelucreaza. El extrage numerele care sunt putere perfecta de acest id. Numerele se afla in seturile de pe coloanele matricii de seturi.

Aceste seturi se adauga la un set si marimea setului se scrie in fisierul de out.