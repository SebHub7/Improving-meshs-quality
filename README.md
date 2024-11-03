Ce projet consiste en l'insertion de points et à la triangulation progressive au fur et à mesure des insertions. La triangulation de Delaunay est utilisée afin de créer des triangles les plus équilatéraux possible. Ce code peut être utilisé par exemple pour le maillage d'un terrain que l'on scannerai.

Exemple:
Voici un maillage après avoir inséré des points et construit le maillage au fur et à mesure, le problème est que nos points peuvent être n'importe ou ce qui donne ce résultat:
![image](https://github.com/user-attachments/assets/be9f0bf1-ca7d-4a75-bc31-fb030067afb6)

Afin d'obtenir un maillage correct nous pouvons apres chaque insertion de point faire un "edge flip" afin que chaque triangle soit de Delaunay:
![image](https://github.com/user-attachments/assets/88b51a5a-4f8f-4fb1-ae28-cc10d4fea1ea)
