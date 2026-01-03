SELECT DISTINCT person_id
FROM person_visits AS pv
WHERE pv.visit_date BETWEEN '2022-01-06' AND '2022-01-09'
OR pv.pizzeria_id = 2
ORDER BY pv.person_id DESC;
