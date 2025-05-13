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

    ### Uporabljena knjižnica mongocxx
[Mongo dokumentacija](https://cloud.mongodb.com/v2/67eba2f3cda2c84940f02ab6#/overview?connectCluster=Cluster0)