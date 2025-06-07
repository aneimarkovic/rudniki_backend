# DATABASE HANDLER DOKUMENTACIJA
## Atributi:
- `instance`:
    Instanca podatkovne baze.
- `uri`:
    hrani connection string (za povezavo na podatkovno bazo).
- `clientOptions`:
    nastavitve povezave podatkovne baze.
- `connection`:
    hrani povezavo z podatkovno bazo.
- `db`:
    pove z katero podatkovno bazo smo povezani.

## Metode:
- `Konstruktor`
    ustvari instanco podatkovne baze, povezavo in db objekt (poveže z bazo in pinga), preveri ali je povezava vzpostavljena.
- `Insert Document`
    prejme ime collectiona, in podatke že v formatu za vstavljanje in vstavi v bazo.
- `insertDocumentGetInsertId`
    Vstavi dokument in vrne insert id namesto bool
- `updateOneItem`
   Posodobi eno vrednosti v dokumentu
- `fetchSingleDocument`
    Pridobi en dokument, če ga najde
- `fetchMultipleDocuments`
    Pridobi več dokumentov, če jih najde
- `fetchMultipleDocumentsAggregate`
    Pridobi več dokumentov, na podlagi funkcije aggregate,ki se uporablja za min,max,avg...
- `deleteDocument`
  Funkcija za brisanje dokumenta
- `updateOneItem`
  Funkcija, ki updata en dokument
- `getSpecificColumnFromDocument`
  Funkcija, ki prejme stolpec in filter in vrne tisti stolpec.
- `create2dsphereIndex`
  Funkcija ustvari 2dshere index za tabelo, ki je potrebene za izvajenje geospatiol queries

  ### Uporabljena knjižnica mongocxx
[Mongo dokumentacija](https://cloud.mongodb.com/v2/67eba2f3cda2c84940f02ab6#/overview?connectCluster=Cluster0)