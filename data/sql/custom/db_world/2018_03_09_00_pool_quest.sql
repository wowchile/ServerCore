update quest_template_addon set ExclusiveGroup = 0 where id in (select entry from pool_quest where pool_entry in (384,385,5678));

delete from pool_quest where pool_entry in (384,385,5678); -- Wintergrasp y semanales de Dalaran (Must Die!)