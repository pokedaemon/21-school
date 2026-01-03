SELECT
	(
	person.name || ' (age:' || CAST(person.age AS VARCHAR) ||  
	',gender:''' || person.gender || 
	''',address:''' || person.address || ''')'
	)
AS person_information
FROM person
ORDER BY person_information ASC;