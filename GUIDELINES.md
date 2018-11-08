# Pautas de desarrollo

Para contribuir con este proyecto es OBLIGATORIO cumplir con el Código de Conducta y las pautas de desarrollo descritas en el presente documento. El no cumplir con estas exigencias le da el derecho a los mantenedores del repositorio a cerrar cualquier *pull request* sin mayor explicación.

## Estilo de programación

### Nombres de clases, atributos, variables, constantes y funciones

### Comentarios y documentación 

### Llaves, espacios e identación

### Formas cortas

### Longitud de líneas y métodos

## Pautas para el uso de Git

El control de cambios del código será realizado con Git usando como único origen el presente repositorio alojado en GitHub. Los mantenedores del proyecto se comprometen a indicar todas las reglas y procedimientos necesarios para contribuir con el proyecto, pero no entregarán soporte técnico u orientación con respecto a el **cómo** trabajar Git o **cuáles** herramientas utilizar. Es responsabilidad de cada contribuidor el aprender a utilizar Git y elegir las herramientas que más le acomoden.

### Branching

Para el manejo de ramas (branches), utilizaremos el modelo GitFlow descrito en [A successful Git branching model](https://nvie.com/posts/a-successful-git-branching-model/). De esta forma, organizaremos trabajo de la siguiente forma:

* **master** branch: 
    
    Sólo versiones estables listas para producción, también llamadas releases. Los tags de versiones se asignarán al hacer un merge desde un branch release-* o hotfix-*.

* **develop** branch: 
    
    El branch principal. Contiene todos los cambios realizados para la versión en curso según planificación, o eventuales hotfixes.

* **feature-xx** branch: 

    Contiene los cambios específicos para un área del juego. En el caso específico de este repositorio, organizaremos los features por:
    - `raids y dungeons`: un branch por instancia. Ejemplos: feature-icc, feature-rubysanctum, feature-pitofsaron
    - `clases`: hechizos específicos de cada clase, se creará un branch por clase. Ejemplos: feature-paladin, feature-rogue, feature-mage
    - `mecánicas`: atributos de los personajes (armadura, penetración de armadura, diminishing return, etc.). Para estos casos se utilizará de forma única la rama **feature-mechanics**
    - `quests`: todo sobre las misiones y sus componentes involucrados (npc, spells, items, etc.). Para estos casos se utilizará de forma única la rama **feature-quests**
    - `NPC`: todos los npc (animales, humanoides, etc.) que se encuentren en el *open world* y no estén relacionadas a ninguna misión. Ejemplos de estos son: vendedores, entrenadores de clase/profesión, guardias, etc. Para estos casos se utilizará de forma única la rama **feature-npc**
    - `otros`: cualquier otra funcionalidad que se desee agregar o modificar en el juego y que no se encuentre dentro de las categorías anteriores. Se deberá asignar un nombre descriptivo pero no mayor a 15 caracteres (sin incluir el `feature-`). Ejemplos: feature-transmog, feature-guildhouse, feature-morph, feature-vmaps.

* **hotfix-vX.Y.Z** branch: 

    Contienen todas las correcciones(bugs) de último momento **para las versiones publicadas en master**. Las correcciones para **develop** o **release-xx** deben realizarse directamente en sus respectivos branches, sin usar un nuevo branch. Cada branch *hotfix* deberá ser enviado al *master branch*, *develop branch* y *release branch activa* para mantener la consistencia entre todas las ramas. El nombre debe hacer referencia a la versión del release que se está corrigiendo, siendo X=versión mayor, Y=versión menor, Z=parche. Al hacer un merge de un hotfix, se incrementará la versión del parche.

* **release-vX.Y** branch: 
    
    Versión candidata a ser publicada en master. Se realizarán releases según planificación interna del equipo del proyecto. La creación de un *release branch* no significa el corte definitivo de la versión, ya que se podrían incluir nuevos cambios/hotfixes desde *develop*.

### Commits

### Pull Request

