/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author IFT-2008, Étudiant(e)
* \version 0.5
* \date avril 2021
*
* Travail pratique numéro 3.
*
*/

#include "DicoSynonymes.h"
#include <vector>
#include <algorithm>

namespace TP3
{

    /**
     * \fn	DicoSynonymes::DicoSynonymes()
     * \brief constructeur par defaut. initialisation de l'attribut racine, nbRadicaux et groupesSynonymes.
     */
    DicoSynonymes::DicoSynonymes()
    {
        racine = nullptr;
        nbRadicaux = 0;
        groupesSynonymes = std::vector<std::list<NoeudDicoSynonymes*>>();
    }

    /**
     * \fn	DicoSynonymes::DicoSynonymes()
     * \brief constructeur avec parametre a partir d'un fichier. Chargement du dictionnaire a partir du fichier en parametre.
     */
    DicoSynonymes::DicoSynonymes(std::ifstream &fichier)
    {
        racine = nullptr;
        nbRadicaux = 0;
        groupesSynonymes = std::vector<std::list<NoeudDicoSynonymes*>>();
        chargerDicoSynonyme(fichier);
    }

    /**
     * \fn	DicoSynonymes::DicoSynonymes()
     * \brief constructeur avec parametre a partir d'un fichier. Chargement du dictionnaire a partir du fichier en parametre.
     */
    DicoSynonymes::~DicoSynonymes()
    {
        _destructeur(racine);
    }

    void DicoSynonymes::ajouterRadical(const std::string& motRadical)
    {
        _auxAjouterRadical(racine, motRadical);
    }

    void DicoSynonymes::ajouterFlexion(const std::string& motRadical, const std::string& motFlexion)
    {
        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, motRadical);

        if(noeudRadical == nullptr)
            throw std::logic_error("DicoSynonymes::ajouterFlexion: Le radical n'est pas dans l'arbre!");

        for(std::string flexion : noeudRadical->flexions)
        {
            if(flexion == motFlexion)
                throw std::logic_error("DicoSynonymes::ajouterFlexion: La flexion existe deja !");
        }

        noeudRadical->flexions.push_back(motFlexion);
    }

    void DicoSynonymes::ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe)
    {
        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, motRadical);

        //Verifier si le radical existe
        if(noeudRadical == nullptr)
            throw std::logic_error("DicoSynonymes::ajouterSynonyme: Le radical n'est pas dans l'arbre!");

        if(numGroupe == -1)
        {
            //Creation du nouveau groupe de synonymes
            groupesSynonymes.push_back(std::list<NoeudDicoSynonymes*>());
            numGroupe = groupesSynonymes.size() - 1;
            noeudRadical->appSynonymes.push_back(numGroupe);
        }

        //Verifier si le numGroupe appartient a appSynonyme du radical
        if(std::find(noeudRadical->appSynonymes.begin(),
                     noeudRadical->appSynonymes.end(), numGroupe) != noeudRadical->appSynonymes.end())
        {
            for(NoeudDicoSynonymes* noeud : groupesSynonymes[numGroupe])
                if(noeud->radical == motSynonyme)
                    throw std::logic_error("DicoSynonymes::ajouterSynonyme: Le synonyme pour le motRadical est deja present!");

            //Si le synonyme n'est pas present dans larbre, l'ajouter dans l'arbre pour l'ajouter dans le groupe
            if(_auxAppartient(racine, motSynonyme) == nullptr)
                _auxAjouterRadical(racine, motSynonyme);

            NoeudDicoSynonymes* noeudSynonyme = _auxAppartient(racine, motSynonyme);
            groupesSynonymes[numGroupe].push_back(noeudSynonyme);
            groupesSynonymes[numGroupe].push_back(noeudRadical);
            noeudSynonyme->appSynonymes.push_back(numGroupe);
        }
        else{throw std::logic_error("DicoSynonymes::ajouterSynonyme: le numGroupe n'est pas present dans appSynonymes du motRadical!");}
    }

    void DicoSynonymes::supprimerRadical(const std::string& motRadical)
    {
        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, motRadical);

        if(noeudRadical == nullptr)
            throw std::logic_error("DicoSynonymes::supprimerRadical: Le radical n'est pas dans l'arbre !");

        std::vector<int> appSynonymeRadical = noeudRadical->appSynonymes;
        for(int groupe : appSynonymeRadical)
        {
            supprimerSynonyme(motRadical, motRadical, groupe);
        }
        _auxSupprimerRadical(racine, motRadical);
    }

    void DicoSynonymes::supprimerFlexion(const std::string& motRadical, const std::string& motFlexion)
    {
        if(_auxAppartient(racine, motRadical) == nullptr || motFlexion.empty())
            throw std::logic_error("DicoSynonymes::supprimerFlexion: Le radical n'est pas dans l'arbre ou la flexion est incorrect!");
        _auxAppartient(racine, motRadical)->flexions.remove(motFlexion);
    }

    void DicoSynonymes::supprimerSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe)
    {
        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, motRadical);
        NoeudDicoSynonymes* noeudSynonyme = _auxAppartient(racine, motSynonyme);

        if(noeudRadical == nullptr || noeudSynonyme == nullptr)
            throw std::logic_error("DicoSynonymes::supprimerSynonyme: Le radical ou le synonyme n'est pas dans l'arbre");

        bool radicalAppGroupe = false;
        for(int groupe : noeudRadical->appSynonymes)
        {
            if(groupe == numGroupe)
                radicalAppGroupe = true;
        }
        if(!radicalAppGroupe)
            throw std::logic_error("DicoSynonymes::supprimerSynonyme: Le radical n'appartient pas au groupe numGroupe");

        for(NoeudDicoSynonymes* noeud : groupesSynonymes[numGroupe])
        {
            if(noeud == noeudSynonyme)
            {
                groupesSynonymes[numGroupe].remove(noeud);
                noeud->appSynonymes.
                erase(std::remove(noeud->appSynonymes.begin(),noeud->appSynonymes.end(), numGroupe),
                      noeud->appSynonymes.end());
                break;
            }
        }
    }

    bool DicoSynonymes::estVide() const
    {
        return nbRadicaux == 0;
    }

    int DicoSynonymes::nombreRadicaux() const
    {
        return nbRadicaux;
    }


    // Méthode fournie
    void DicoSynonymes::chargerDicoSynonyme(std::ifstream& fichier)
    {
        if (!fichier.is_open())
            throw std::logic_error("DicoSynonymes::chargerDicoSynonyme: Le fichier n'est pas ouvert !");

        std::string ligne;
        std::string buffer;
        std::string radical;
        int cat = 1;

        while (!fichier.eof()) // tant qu'on peut lire dans le fichier
        {
            std::getline(fichier, ligne);

            if (ligne == "$")
            {
                cat = 3;
                std::getline(fichier, ligne);
            }
            if (cat == 1)
            {
                radical = ligne;
                ajouterRadical(radical);
                cat = 2;
            }
            else if (cat == 2)
            {
                std::stringstream ss(ligne);
                while (ss >> buffer)
                    ajouterFlexion(radical, buffer);
                cat = 1;
            }
            else
            {
                std::stringstream ss(ligne);
                ss >> radical;
                ss >> buffer;
                int position = -1;
                ajouterSynonyme(radical, buffer, position);
                while (ss >> buffer)
                    ajouterSynonyme(radical, buffer, position);
            }
        }
    }

    std::string DicoSynonymes::rechercherRadical(const std::string& mot) const
    {
        return "";
    }

    float DicoSynonymes::similitude(const std::string& mot1, const std::string& mot2) const
    {
        return 0;
    }

    int DicoSynonymes::getNombreSens(std::string radical) const
    {
        return _auxAppartient(racine, radical)->appSynonymes.size();
    }

    std::string DicoSynonymes::getSens(std::string radical, int position) const
    {
        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, radical);

        if(noeudRadical == nullptr)
            throw std::logic_error("DicoSynonymes::getSens: Le radical n'est pas dans l'arbre !");
        if(position > noeudRadical->appSynonymes.size() - 1)
            throw std::logic_error("DicoSynonymes::getSens: La position entree n'est pas valide");

        return groupesSynonymes[noeudRadical->appSynonymes[position]].front()->radical;
    }

    std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const
    {
        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, radical);
        if(noeudRadical == nullptr)
            throw std::logic_error("DicoSynonymes::getSynonymes: Le radical n'est pas dans l'arbre ! ");
        if(position > noeudRadical->appSynonymes.size() - 1)
            throw std::logic_error("DicoSynonymes::getSynonymes: La position entree n'est pas valide");

        std::list<NoeudDicoSynonymes*> synonymesList = groupesSynonymes[noeudRadical->appSynonymes[position]];
        std::vector<std::string> synonymesVec;

        for(NoeudDicoSynonymes* noeudSynonyme : synonymesList)
            synonymesVec.push_back(noeudSynonyme->radical);

        return synonymesVec;
    }

    std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const
    {
        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, radical);
        if(noeudRadical == nullptr)
            throw std::logic_error("DicoSynonymes::getFlexions: Le radical n'est pas dans l'arbre !");

        std::vector<std::string> flexionsVec;

        for(std::string const &flexion : noeudRadical->flexions)
            flexionsVec.push_back(flexion);

        return flexionsVec;
    }




    /////////////////////// METHODES PRIVEES //////////////////////////

    void DicoSynonymes::_destructeur(NoeudDicoSynonymes *&noeud)
    {
        if(noeud != nullptr)
        {
            _destructeur(noeud->gauche);
            _destructeur(noeud->droit);
            delete noeud;
        }
    }

    void DicoSynonymes::_auxAjouterRadical(NoeudDicoSynonymes *&noeud, const std::string &motRadical)
    {
        if(noeud == nullptr)
        {
            noeud = new NoeudDicoSynonymes(motRadical);
            nbRadicaux++;
            return;
        }
        if(motRadical < noeud->radical)
            _auxAjouterRadical(noeud->gauche, motRadical);
        else if(motRadical > noeud->radical)
            _auxAjouterRadical(noeud->droit, motRadical);
        else
            throw std::logic_error("DicoSynonymes::_auxAjouterRadical: Le radical existe deja !");

        _balance(noeud);
    }

    void DicoSynonymes::_auxSupprimerRadical(NoeudDicoSynonymes *&arbre, const std::string &motRadical)
    {
        if(arbre == nullptr)
            throw std::logic_error("DicoSynonymes::_auxSupprimerRadical: Le radical a supprimer est absent!");
        if(motRadical < arbre->radical)
            _auxSupprimerRadical(arbre->gauche, motRadical);
        else if(motRadical > arbre->radical)
            _auxSupprimerRadical(arbre->droit, motRadical);
        else if(arbre->gauche != nullptr && arbre->droit != nullptr)
            _supprimerSuccMinDroite(arbre);
        else
        {
            NoeudDicoSynonymes* vieuxNoeud = arbre;
            arbre = (arbre->gauche != nullptr) ? arbre->gauche : arbre->droit;
            delete vieuxNoeud;
            --nbRadicaux;
        }
         _balance(arbre);
    }

    void DicoSynonymes::_supprimerSuccMinDroite(NoeudDicoSynonymes *arbre)
    {
        NoeudDicoSynonymes* temp = arbre->droit;
        NoeudDicoSynonymes* parent = arbre;

        while (temp->gauche != 0)
        {
            parent = temp;
            temp = temp->gauche;
        }

        arbre->radical = temp->radical;
        std::swap(arbre->appSynonymes, temp->appSynonymes);
        std::swap(arbre->flexions, temp->flexions);

        if(temp == parent->gauche)
            _auxSupprimerRadical(parent->gauche, temp->radical);
        else
            _auxSupprimerRadical(parent->droit, temp->radical);
    }

    void DicoSynonymes::_balance(NoeudDicoSynonymes* &arbre)
    {
        if(_debalancementAGauche(arbre))
        {
            if(_sousArbrePencheADroite(arbre->gauche))
                _zigZagGauche(arbre);
            else
                _zigZigGauche(arbre);
        } else if(_debalancementADroite(arbre))
        {
            if(_sousArbrePencheAGauche(arbre->droit))
                _zigZagDroit(arbre);
            else
                _zigZigDroit(arbre);
        }
        else
        {
            if(arbre != nullptr)
                arbre->hauteur = 1 + std::max(_hauteur(arbre->gauche), _hauteur(arbre->droit));
        }
    }

    int DicoSynonymes::_hauteur(NoeudDicoSynonymes *arbre)
    {
        if(arbre == nullptr)
            return -1;
        return arbre->hauteur;
    }

    bool DicoSynonymes::_debalancementAGauche(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == nullptr)
            return false;
        if(1 < _hauteur(arbre->gauche) - _hauteur(arbre->droit))
            return true;
        return false;
    }

    bool DicoSynonymes::_debalancementADroite(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == nullptr)
            return false;
        if(1 < _hauteur(arbre->droit) - _hauteur(arbre->gauche))
            return true;
        return false;
    }

    bool DicoSynonymes::_sousArbrePencheADroite(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == nullptr)
            return false;
        if(_hauteur(arbre->gauche) < _hauteur(arbre->droit))
            return true;
        return false;
    }

    bool DicoSynonymes::_sousArbrePencheAGauche(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == nullptr)
            return false;
        if(_hauteur(arbre->droit) < _hauteur(arbre->gauche))
            return true;
        return false;
    }

    void DicoSynonymes::_zigZigGauche(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == racine)
        {
            NoeudDicoSynonymes *noeud = racine->gauche;
            racine->gauche = noeud->droit;
            noeud->droit = racine;
            racine->hauteur = 1 + std::max(_hauteur(racine->gauche), _hauteur(racine->droit));
            noeud->hauteur = 1 + std::max(_hauteur(noeud->gauche), racine->hauteur);
            racine = noeud;
        }
        else
        {
            NoeudDicoSynonymes *noeud = arbre->gauche;
            arbre->gauche = noeud->droit;
            noeud->droit = arbre;
            arbre->hauteur = 1 + std::max(_hauteur(arbre->gauche), _hauteur(arbre->droit));
            noeud->hauteur = 1 + std::max(_hauteur(noeud->gauche), arbre->hauteur);
            arbre = noeud;
        }
    }

    void DicoSynonymes::_zigZigDroit(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == racine)
        {
            NoeudDicoSynonymes *noeud = racine->droit;
            racine->droit = noeud->gauche;
            noeud->gauche = racine;
            racine->hauteur = 1 + std::max(_hauteur(racine->droit), _hauteur(racine->gauche));
            noeud->hauteur = 1 + std::max(_hauteur(noeud->droit), racine->hauteur);
            racine = noeud;
        }
        else
        {
            NoeudDicoSynonymes *noeud = arbre->droit;
            arbre->droit = noeud->gauche;
            noeud->gauche = arbre;
            arbre->hauteur = 1 + std::max(_hauteur(arbre->droit), _hauteur(arbre->gauche));
            noeud->hauteur = 1 + std::max(_hauteur(noeud->droit), arbre->hauteur);
            arbre = noeud;
        }

    }

    void DicoSynonymes::_zigZagGauche(NoeudDicoSynonymes *&arbre)
    {
        _zigZigDroit(arbre->gauche);
        _zigZigGauche(arbre);
    }

    void DicoSynonymes::_zigZagDroit(NoeudDicoSynonymes *&arbre)
    {
        _zigZigGauche(arbre->droit);
        _zigZigDroit(arbre);
    }

    DicoSynonymes::NoeudDicoSynonymes * DicoSynonymes::_auxAppartient(NoeudDicoSynonymes *arbre, const std::string &radical) const
    {
        if(arbre == nullptr)
            return nullptr;

        if(radical < arbre->radical)
            return _auxAppartient(arbre->gauche, radical);
        else if(radical > arbre->radical)
            return _auxAppartient(arbre->droit, radical);
        else
            return arbre;
    }

}//Fin du namespace