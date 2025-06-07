# ModelTemplate Dokumentacija
## Na hitro
Implemetacija User modela za mongodb bazo.

## Atributi:
- `username` 
- `password` 
- `email` 
- `birthDate`

## Metode:

- `getFromBsonDocument`
     Funkcija zgradi UserModel objekt iz pridobljenega BSON dokumenta
- `convertToBsonDocument`
    Funkcija zgradi BSON dokument iz trenutnega UserModel objekta
- `checkIfUserIsLoggedIn`
    Preveri ali je uporabnik prijavljen
- `getUserIdFromJWT`
    Iz jwt pridobi userid
- `getBsonDocumentLogn`
    Pridobi login podatke iz bson dokument (username ali email in password)
- `hashPassword`
    Zakodira geslo uporabnika
- `verifyPassword`
    Preveri ali je geslo pravilno (zakodirano in plain primerja)
    
### Uporabljena knjižnica Boost.beast
[Boost dokumentacija](https://live.boost.org/)
