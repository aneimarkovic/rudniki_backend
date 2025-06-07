# UserController dokumentacija

## Atributi:

## Metode:

- `saveUser`
    Registracije uporabnika(shrani novega uporabnika v tabelo in ustvari jwt)
- `loginUser`
    Prijavi upodabnika (preveri username/email in geslo), če je ok vrne userid če ne pa sporočilo
- `getUser`
    Pridobi uporabnika, na podlagi njegovega id
- `updateUser`
    Posodobi uporabnikove podatke