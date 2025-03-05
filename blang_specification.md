# Spécification de la Bibliothèque BLang

## Introduction

La bibliothèque BLang est une implémentation en C++ pour représenter et manipuler des artefacts du langage B. Elle fournit une représentation efficace et thread-safe des différentes catégories syntaxiques et des types B avec un partage maximal des sous-éléments pour optimiser l'utilisation de la mémoire.

## Représentation du système de types

### Objectifs

- Fournir une représentation complète du système de types du langage B
- Assurer la thread-safety pour une utilisation dans des environnements multi-threads
- Implémenter le partage maximal des sous-types pour une utilisation efficace de la mémoire
- Offrir des mécanismes de sérialisation et désérialisation XML
- Fournir un formattage compatible avec la bibliothèque `fmt`
- Faciliter la manipulation des types via un modèle de visiteur

### Types Supportés

#### Types de Base

- **INTEGER** : Représente le type entier
- **BOOLEAN** : Représente le type booléen
- **FLOAT** : Représente le type flottant
- **REAL** : Représente le type réel
- **STRING** : Représente le type chaîne de caractères

#### Types Complexes
- **ProductType** (Type Produit) : Représente le produit cartésien de deux types
- **PowerType** (Type Ensemble de Parties) : Représente l'ensemble des parties d'un type
- **StructType** (Type Structure) : Représente une structure avec des champs nommés
- **AbstractSet** (Ensemble Abstrait) : Représente un ensemble abstrait défini par un nom
- **EnumeratedSet** (Ensemble Énuméré) : Représente un ensemble énuméré avec des valeurs nommées

### Architecture

#### Classes Principales

- **BLang::Type** : Classe abstraite de base pour tous les types
  - Fournit une interface commune pour tous les types
  - Implémente les méthodes de comparaison et de hachage
  - Définit les classes imbriquées pour les types spécifiques

- **BLang::TypeFactory** : Fabrique pour créer et gérer les instances de types
  - Assure le partage maximal des sous-types
  - Fournit des méthodes statiques pour créer chaque type
  - Maintient un index des types pour un accès efficace

#### Fonctionnalités Supplémentaires

- **Visiteur** : Implémentation du modèle de conception Visiteur pour les opérations spécifiques aux types
- **Formattage** : Support pour la bibliothèque `fmt` via des adaptateurs spécialisés
- **Sérialisation XML** : Mécanismes pour écrire et lire des types au format XML
- **Thread-Safety** : Garantie de sécurité dans les environnements multi-threads

## Dépendances

- **fmt** : Bibliothèque de formatage de texte
- **tinyxml2** : Bibliothèque de parsing XML
- **Threads** : Support pour la programmation multi-threads

## Compilation et Installation

### Prérequis

- Compilateur C++ supportant C++17
- CMake 3.14 ou supérieur
- Bibliothèques fmt et tinyxml2

### Compilation

```sh
mkdir build
cd build
cmake ..
make
```

### Installation

```sh
make install
```

## Tests

La bibliothèque inclut une suite de tests complète pour vérifier :
- La création et manipulation des types de base
- La création et manipulation des types complexes
- Les comparaisons de types
- La thread-safety
- Le partage maximal
- La cohérence du hachage
- Le modèle de visiteur
- Le formatage
- La sérialisation et désérialisation XML

## 9. Licence

BTYPE est un logiciel libre distribué sous les termes de la licence GNU General Public License version 3 ou ultérieure. 