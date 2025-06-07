# MineController dokumentacija

## Atributi:

## Metode:

- `saveMine`:
  Shrani novi rudnik v bazo
- `getMine`:
  Pridobi rudnik iz baze
- `deleteMine`:
  Izbriše Rudnik
- `addInfrastructure`:
  Doda novo infrastrukturo v rudnik
- `addMineral`:
  Doda nove minerale v rudnik
- `addWorker`:
  Doda nove delavce v rudnik
- `generateMineralsValue`
  Pridobi točko, generira krožnico radija 5km okrog točke in pridobi vse minerale v tej krožnici
- `getScrapperMines`
  Vrne vse rudnika, ki smo jih pridobili z scrapperjem
- `getFilteredMines`
  Funkcija, ki vrne vse filtrirane rudnike
- `getMineBasedOnOwner`
  Funkcija, ki vrne vse uporabnikove rudnike
- `getAllMines`
  Funkcija, ki vrne vse rudnike
- `searchBar`
  Funkcija, ki prejme niz in najde rudnik, ki se prilega temu nizu
- `getMinesByYear`
  Funkcija, ki pridobi rudnike v določenem časovnem intervalu
- `deleteWorker`
  Funkciaj za brisanje delavcev rudnika
- `deleteInfrastructure`
  Funkciaj za brisanje infrastrukture rudnika
- `deleteMineral`
  Funkciaj za brisanje mineralov rudnika
- `updateMine`
  Funkcija za posodabljanje podatkov rudnika
- `getStatistics`
  Funkcija izračuna statistične podakte o rudnikih in jih pošlje odjemalcu