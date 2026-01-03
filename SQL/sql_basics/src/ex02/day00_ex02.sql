SELECT name, rating
FROM pizzeria as pz
WHERE pz.rating >= 3.5 AND pz.rating <= 5.0
ORDER BY pz.rating ASC;

SELECT name, rating
FROM pizzeria as pz
WHERE pz.rating BETWEEN 3.5 AND 5.0
ORDER BY pz.rating ASC;