SELECT DISTINCT
	(SELECT p.name FROM person p WHERE p.id = po.person_id) AS NAME
FROM person_order po
WHERE (
	po.id = 13 or
	po.id = 14 or
	po.id = 18
	)
AND po.order_date = '2022-01-07'
	