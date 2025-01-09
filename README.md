
# Visualisateur de Processus (plist.exe)

Ce projet, réalisé durant mon cursus de 1ère année à l'école 2600, propose un outil en ligne de commande pour lister et analyser les processus et threads en cours d'exécution sur un système Windows. Il utilise l'API Windows ToolHelp pour collecter des informations détaillées sur les processus et threads, notamment le temps CPU, le temps écoulé et les priorités.

## Fonctionnalités

- **Lister tous les processus** : Affiche des informations de base sur tous les processus en cours d'exécution.
- **Filtrer par nom de processus** : Affiche les informations relatives à un processus spécifique en fonction de son nom.
- **Affichage détaillé des threads** : Montre des informations détaillées sur les threads d'un processus spécifique, identifié par son PID (Process ID).

## Utilisation

```plaintext
plist.exe [options]
```

### Options

- `-h` : Affiche ce message d'aide.
- `[NomDuProcessus]` : Affiche les informations relatives au processus correspondant au nom spécifié.
- `-d [PID]` : Affiche des informations détaillées sur les threads du processus avec le PID spécifié.

### Exemple d'utilisation

1. Lister tous les processus :
   ```plaintext
   plist.exe
   ```

2. Filtrer par nom de processus :
   ```plaintext
   plist.exe notepad.exe
   ```

3. Afficher les threads détaillés d'un processus :
   ```plaintext
   plist.exe -d 1234
   ```

## Compilation

Ce programme est écrit en C et doit être compilé sous Windows. Utilisez un compilateur comme `gcc` ou celui intégré "par défaut" dans Windows via VS Code avec `cl.exe`.

### Exemple de compilation :

```bash
gcc -o plist.exe plist.c
```

```bash
cl.exe plist.c
```

## Notes Techniques

- Utilise l'API ToolHelp pour capturer des instantanés des processus et threads.
- Nécessite des privilèges suffisants pour interroger les processus système.
- Ce code présente encore des bugs, comme certaines données non valides ou non récupérées. 

## Licence

Ce projet est distribué sous une licence MIT.
