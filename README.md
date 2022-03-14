# Saint_Builder

![alt text](https://raw.githubusercontent.com/DallasFR/Saint_Builder/main/images/saint_michel_diable.jpg)

Petit builder que j'ai fait qui permet de générer automatiquement une payload fonctionnelle via un fichier bin. 

runner --> Template

Saint_Builder --> Builder

Le stub est basique, un anti-debug basique (IsDebuggerPresent) tout comme l'anti-vm qui est un Sleep suivis d'un GetTickCount.
Pour utiliser le tools, vous lancez le builder et vous spécifiez votre payload en raw, pour les petites payloads de quelques centaines de bytes ajouté des nops.
La clé xor est hardcodé, tous le monde à la même. Le sleep aussi et le call de GetTickCount obfusqué aussi.

De plus, le fait de xorer une payload dans les ressources du PE vous rend vulnérable à une attaque par clair connus, mais bon, je pense que c'est plus rapide de debug/reverse pour trouver la payload original.
Pour l'injection, rien de sorcier, on est sur du self-inject avec des syscalls (merci SysWhispers3).

Si vous voulez des pistes pour améliorer le payload je vous conseille :

- Revoir le chiffrement de la payload
- Ajouter des anti-vm, anti-dbg, anti-vm digne de ce nom (checkpoint à de très bons articles dessus)
- Revoir l'injection, je crée un thread, on peut faire mieux genre EarlyBird pour executer la payload via APC ou abuser de fonctions windows (http://ropgadget.com/posts/abusing_win_functions.html)

# Demo

![alt text](https://raw.githubusercontent.com/DallasFR/Saint_Builder/main/images/gif_exemple.gif)
