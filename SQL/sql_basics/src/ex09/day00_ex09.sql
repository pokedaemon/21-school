SELECT
	(
		SELECT name FROM person pe
		WHERE pe.id = pv.person_id
	) AS person_name,
	(
		SELECT name FROM pizzeria pi
		WHERE pi.id = pv.pizzeria_id
	) AS pizzeria_name
FROM (
	SELECT person_id, pizzeria_id, visit_date FROM person_visits pv
) AS pv
WHERE pv.visit_date BETWEEN '2022-01-07' AND '2022-01-09'
ORDER BY person_name ASC, pizzeria_name DESC;