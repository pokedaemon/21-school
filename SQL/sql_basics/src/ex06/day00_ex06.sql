SELECT
	(SELECT p.name FROM person p WHERE p.id = po.person_id) AS name,
	(
	SELECT 
	CASE
		WHEN name = 'Denis' THEN True
		ELSE False
	END
	FROM person p
	WHERE p.id = po.person_id
	) AS check_name
FROM person_order po
WHERE (
	po.id = 13 or
	po.id = 14 or
	po.id = 18
	)
AND po.order_date = '2022-01-07'
	