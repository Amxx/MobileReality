
#ifndef _MESH_H
#define _MESH_H

#include <vector>
#include <string>

#include "Vec.h"
#include "Triangle.h"


namespace gk {

//! description d'une matiere.
//! R= kd * diffuse_color * diffuse_texture + ks * specular_color * specular_texture.
struct MeshMaterial
{
    std::string name;                   //!< nom de la matiere.
    std::string diffuse_texture;        //!< nom de l'image a charger.
    std::string specular_texture;       //!< nom de l'image a charger.
    VecColor diffuse_color;             //!< couleur diffuse.
    VecColor specular_color;            //!< couleur speculaire.
    VecColor emission;                  //!< energie emise par une source de lumiere.
    float kd;                           //!< influence du comportement diffus.
    float ks;                           //!< influence du comportement speculaire.
    float ns;                           //!< comportement speculaire, ks * cos**ns
    float ni;                           //!< indice de refraction, objets transparents
    
    //! constructeur par defaut.
    MeshMaterial( )
        :
        name("default"),
        diffuse_texture(), specular_texture(),
        diffuse_color(0.8f, 0.8f, 0.8f), specular_color(0.0f, 0.0f, 0.0f), emission(0.0f, 0.0f, 0.0f, 0.0f),
        kd(1.0f), ks(0.0f)
    {}
    
    //! construction d'une matiere nommee.
    MeshMaterial( const char *_name )
        :
        name(_name),
        diffuse_texture(), specular_texture(),
        diffuse_color(), specular_color(), emission(),
        kd(0.0f), ks(0.0f)
    {}
};


//! representation d'un groupe de faces associe a une matiere.
struct MeshGroup
{
    MeshMaterial material;
    unsigned int begin;
    unsigned int end;
    
    MeshGroup( ) : material(), begin(0), end(0) {}
    MeshGroup( const MeshMaterial&  _material, const unsigned int _begin ) : material(_material), begin(_begin), end(_begin) {}
};

//! representation d'un ensemble de triangles eventuellement associes a des matieres.
struct Mesh
{
    std::string filename;               //!< nom du fichier.
    
    std::vector<Vec3> positions;        //!< position des sommets.
    std::vector<Vec3> texcoords;        //!< coordonnees de textures.
    std::vector<Vec3> normals;          //!< normales des sommets.
    
    std::vector<unsigned int> indices;   //!< indexation des sommets de chaque triangle, numTriangles = indices.size() / 3.
    
    std::vector<MeshGroup> groups;      //!< groupes de faces associes a une matiere.
    std::vector<int> materials;         //!< groupes / matieres des triangles.
    
    //! constructeur.
    Mesh( )
        :
        positions(), texcoords(), normals(),
        indices(), groups()
    {}
    
    //! renvoie le nombre de triangles du maillage.
    int triangleCount( ) const
    {
        return (int) indices.size() / 3; 
    }
    
    //! renvoie la matiere associee au triangle id.
    MeshMaterial triangleMaterial( const unsigned id ) const
    {
        if(id > materials.size() || materials[id] < 0)
            return MeshMaterial();      // renvoie une matiere par defaut
        
        return groups[materials[id]].material;
    }
    
    //! renvoie un triangle.
    Triangle triangle( const unsigned int id ) const
    {
        assert(id < indices.size() / 3u);
        
        Point a, b, c;
        unsigned int ia= indices[3u*id];
        if(ia < positions.size())
            a= Point(positions[ia]);
        unsigned int ib= indices[3u*id +1u];
        if(ib < positions.size())
            b= Point(positions[ib]);
        unsigned int ic= indices[3u*id +2u];
        if(ic < positions.size())
            c= Point(positions[ic]);
        
        return Triangle( a, b, c, id );
    }
    
    //! renvoie un pn triangle. (position + normale par sommet).
    PNTriangle pntriangle( const unsigned int id ) const
    { 
        assert(id < indices.size() / 3u);
        
        Normal a, b, c;
        unsigned int ia= indices[3u*id];
        if(ia < normals.size())
            a= Normal(normals[ia]);
        unsigned int ib= indices[3u*id +1u];
        if(ib < normals.size())
            b= Normal(normals[ib]);
        unsigned int ic= indices[3u*id +2u];
        if(ic < normals.size())
            c= Normal(normals[ic]);
        
        Triangle abc= triangle(id);
        return PNTriangle(abc, a, b, c);
    }
    
    //! renvoie un ptn triangle. (position + texcoord + normale par sommet).
    PTNTriangle ptntriangle( const unsigned int id ) const
    { 
        assert(id < indices.size() / 3u);
        
        Point a, b, c;
        unsigned int ia= indices[3u*id];
        if(ia < texcoords.size())
            a= Point(texcoords[ia]);
        unsigned int ib= indices[3u*id +1u];
        if(ib < texcoords.size())
            b= Point(texcoords[ib]);
        unsigned int ic= indices[3u*id +2u];
        if(ic < texcoords.size())
            c= Point(texcoords[ic]);
        
        PNTriangle abc= pntriangle(id);
        return PTNTriangle(abc, a, b, c);
    }
};

}       // namespace

#endif
