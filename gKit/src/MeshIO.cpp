
#include <cstdio>
#include <vector> 
#include <map> 

#include "IOFileSystem.h"

#include "MeshIO.h"
#include "Geometry.h"
#include "Mesh.h"


namespace gk {

namespace OBJ {

//! representation d'un sommet avec un index par attribut.
struct index
{
    int position;
    int texcoord;
    int normal;
    int material;
    
    index( ) : position(-1), texcoord(-1), normal(-1), material(-1) {}
    index( const int _p, const int _t, const int _n, const int _mat ) : position(_p), texcoord(_t), normal(_n), material(_mat) {}
    
    bool operator< ( const index& b ) const
    {
        if(material != b.material)
            return (material < b.material);
        
        // ordre lexicographique pour comparer le triplet d'indices
        if(position != b.position)
            return (position < b.position);
        if(texcoord != b.texcoord)
            return (texcoord < b.texcoord);
        return (normal < b.normal);
    }
};

struct face
{
    int first;
    int n;
    
    face( ) : first(0), n(0) {}
    face( const int f, const int c ) : first(f), n(c) {}
};

struct data
{
    std::vector<Vec3> positions;
    std::vector<Vec3> texcoords;
    std::vector<Vec3> normals;
    std::vector<index> indices;
    std::vector<face> faces;
    
    std::vector<MeshMaterial> materials;
    
    bool has_normal;
    bool has_texcoord;
    bool has_material;
    
};

}       // namespace OBJ


namespace MeshIO {

static 
int attribute( const int index, const int count )
{
    int a= -1;
    if(index < 0)
        a= count + index;       // indexation relative aux derniers sommets
    else
        a= index - 1;           // indexations classique
    
    if(a < 0 || a >= count)
    {
        static int warnings= 0;
        if(warnings == 0)
        {       // beurk
            warnings++;
            printf("invalid index %d, max %d\n", index, count);
        }
        
        a= -1;
    }
    
    return a;
}

#if 0
static
int read( const std::string& filename, data& mesh )
{
    // ouvre le fichier
    FILE *in= fopen(filename.c_str(), "rt");
    if(in == NULL)
    {
        printf("error reading '%s'.\n", filename.c_str());
        return -1;
    }
    
    printf("loading '%s'...\n", filename.c_str());
    
    int material_id= -1;
    int material_base= 0;
    
    char line[1024];
    char tmp[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(fgets(line, sizeof(line), in) == NULL)
        {
            error= false;               // fin du fichier, pas d'erreur detectee
            break;
        }
        
        line[sizeof(line) -1]= 0;       // force la fin de la ligne
        if(line[0] == 'v')
        {
            if(line[1] == ' ')          // position
            {
                float x, y, z;
                if(sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    break;
                mesh.positions.push_back( Vec3(x, y, z) );
            }
            else if(line[1] == 'n')     // normal
            {
                float x, y, z;
                if(sscanf(line, "vn %f %f %f", &x, &y, &z) != 3)
                    break;
                mesh.normals.push_back( Vec3(x, y, z) );
            }
            else if(line[1] == 't')     // texcoord
            {
                float u, v, w;
                int status= sscanf(line, "vt %f %f %f", &u, &v, &w);
                if(status == 2)
                    w= 0.f;             // vt u v, pas de composante w
                if(status >= 2)
                    mesh.texcoords.push_back( Vec3(u, v, w) );
                else
                    break;
            }
        }
        
        else if(line[0] == 'm')
        {
            // charger un ensemble de matieres
            if(sscanf(line, "mtllib %[^\r\n]", tmp) == 1)       // lit toute la ligne, recupere les noms de fichiers avec des espaces.
            {
                material_base= mesh.materials.size();
                readMTL(IOFileSystem::pathname(filename) + tmp, mesh.materials);
            }
        }
        
        else if(line[0] == 'u')
        {
            if(sscanf(line, "usemtl %[^\r\n]", tmp) == 1) // lit toute la ligne, recupere les noms de fichiers avec des espaces.
            {
                // ajouter les faces suivantes au groupe associe a la matiere
                material_id= -1;
                for(unsigned int i= material_base; i < mesh.materials.size(); i++)
                    if(mesh.materials[i].name == tmp)
                    {
                        material_id= i;
                        break;
                    }
            }
        }
        
        else if(line[0] == 'f')         // triangle/quad
        {
            int ia, ita, ina;
            int n;
            
            int count= 0;
            char *next= line +1;
            for(;;)
            {
                n= 0;
                if(sscanf(next, " %d/%d/%d %n", &ia, &ita, &ina, &n) == 3)      // v/t/n format
                    mesh.indices.push_back(OBJ::index(attribute(ia, positions.size()), 
                        attribute(ita, texcoords.size()), 
                        attribute(ina, normals.size()), 
                        material_id));
                
                else if(sscanf(next, " %d//%d %n", &ia, &ina, &n) == 2)         // v//n format
                    mesh.indices.push_back(OBJ::index(attribute(ia, positions.size()), 
                        -1, 
                        attribute(ina, normals.size()), 
                        material_id));
                
                else if(sscanf(next, " %d/%d %n", &ia, &ita, &n) == 2)          // v/t  format
                    mesh.indices.push_back(OBJ::index(attribute(ia, positions.size()), 
                        attribute(ita, texcoords.size()), 
                        -1, 
                        material_id));
                
                else if(sscanf(next, " %d %n", &ia, &n) == 1)                   // v format
                    mesh.indices.push_back(OBJ::index(attribute(ia, positions.size()), 
                        -1, 
                        -1, 
                        material_id));
                
                else
                    break;
                
                // verifie la coherence du modele
                if(material_id < 0)
                    mesh.has_material= false;
                if(mesh.indices.back().normal == -1)
                    mesh.has_normal= false;
                if(mesh.indices.back().texcoord == -1)
                    mesh.has_texcoord= false;
                
                count++;
                next= next +n;
                if(n == 0 || count >= 4)
                    break;
            }
            
            mesh.faces.push_back( face(mesh.faces.size(), count) );
        }
    }
    
    fclose(in);
    
    // cree une matiere par defaut, si necessaire
    if(mesh.indices.empty() == false && mesh.has_material == false)
    {
        mesh.materials.push_back( MeshMaterial() );  
        
        // affecte la matiere par defaut
        for(unsigned int i= 0; i < mesh.indices.size(); i++)
            if(mesh.indices[i].material < 0)
                mesh.indices[i].material= materials.size() -1; 
    }

    return (error ? -1 : 0);
}

static 
void triangulate( const data& mesh, std::vector<OBJ::index>& triangles )
{
    triangles.reserve(mesh.faces.size());
    for(unsigned int i= 0; i < mesh.faces.size(); i++)
    {
        const face& f= mesh.faces[i];
        for(int v= f.first + 2; v < f.first + f.n; v++)
        {
            triangles.push_back( mesh.indices[f.first] ); 
            triangles.push_back( mesh.indices[v -1] );
            triangles.push_back( mesh.indices[v] );
        }
    }
}
#endif

//! analyse un fichier .obj et construit un objet Mesh.
Mesh *readOBJ( const std::string& filename ) 
{
    // ouvre le fichier
    FILE *in= fopen(filename.c_str(), "rt");
    if(in == NULL)
    {
        printf("error reading '%s'.\n", filename.c_str());
        return NULL;
    }
    
    printf("loading '%s'...\n", filename.c_str());
    
    // cree le mesh
    Mesh *mesh= new Mesh;
    
    // indexation des attributs de sommets
    std::vector<Vec3> positions;
    std::vector<Vec3> texcoords;
    std::vector<Vec3> normals;
    std::vector<OBJ::index> indices;
    std::vector<MeshMaterial> materials;
    int material_id= -1;
    int material_base= 0;
    
    bool has_normal= true;
    bool has_texcoord= true;
    bool has_material= true;
    
    char line[1024];
    char tmp[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(fgets(line, sizeof(line), in) == NULL)
        {
            error= false;               // fin du fichier, pas d'erreur detectee
            break;
        }
        
        line[sizeof(line) -1]= 0;       // force la fin de la ligne
        if(line[0] == 'v')
        {
            if(line[1] == ' ')          // position
            {
                float x, y, z;
                if(sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    break;
                positions.push_back( Vec3(x, y, z) );
            }
            else if(line[1] == 'n')     // normal
            {
                float x, y, z;
                if(sscanf(line, "vn %f %f %f", &x, &y, &z) != 3)
                    break;
                normals.push_back( Vec3(x, y, z) );
            }
            else if(line[1] == 't')     // texcoord
            {
                float u, v, w;
                int status= sscanf(line, "vt %f %f %f", &u, &v, &w);
                if(status == 2)
                    w= 0.f;             // vt u v, pas de composante w
                if(status >= 2)
                    texcoords.push_back( Vec3(u, v, w) );
                else
                    break;
            }
        }
        
        else if(line[0] == 'm')
        {
            // charger un ensemble de matieres
            if(sscanf(line, "mtllib %[^\r\n]", tmp) == 1)       // lit toute la ligne, recupere les noms de fichiers avec des espaces.
            {
                material_base= materials.size();
                readMTL(IOFileSystem::pathname(filename) + tmp, materials);
            }
        }
        
        else if(line[0] == 'u')
        {
            if(sscanf(line, "usemtl %[^\r\n]", tmp) == 1) // lit toute la ligne, recupere les noms de fichiers avec des espaces.
            {
                // ajouter les faces suivantes au groupe associe a la matiere
                material_id= -1;
                for(unsigned int i= material_base; i < materials.size(); i++)
                    if(materials[i].name == tmp)
                    {
                        material_id= i;
                        break;
                    }
            }
        }
        
        else if(line[0] == 'f')         // triangle/quad
        {
            OBJ::index face[4];
            int ia, ita, ina;
            int n;
            
            int count= 0;
            char *next= line +1;
            for(;;)
            {
                n= 0;
                if(sscanf(next, " %d/%d/%d %n", &ia, &ita, &ina, &n) == 3)      // v/t/n format
                    face[count]= OBJ::index(attribute(ia, positions.size()), 
                        attribute(ita, texcoords.size()), 
                        attribute(ina, normals.size()), 
                        material_id);
                
                else if(sscanf(next, " %d//%d %n", &ia, &ina, &n) == 2)         // v//n format
                    face[count]= OBJ::index(attribute(ia, positions.size()), 
                        -1, 
                        attribute(ina, normals.size()), 
                        material_id);
                
                else if(sscanf(next, " %d/%d %n", &ia, &ita, &n) == 2)          // v/t  format
                    face[count]= OBJ::index(attribute(ia, positions.size()), 
                        attribute(ita, texcoords.size()), 
                        -1, 
                        material_id);
                
                else if(sscanf(next, " %d %n", &ia, &n) == 1)                   // v format
                    face[count]= OBJ::index(attribute(ia, positions.size()), 
                        -1, 
                        -1, 
                        material_id);
                
                else
                    break;
                
                // verifie la coherence du modele
                //~ if(face[count].material == -1)
                if(material_id < 0)
                    has_material= false;
                if(face[count].normal == -1)
                    has_normal= false;
                if(face[count].texcoord == -1)
                    has_texcoord= false;
                
                count++;
                if(count >= 3)
                {
                    // triangule les polygones convexes de plus 3 sommets
                    indices.push_back(face[0]);
                    indices.push_back(face[count -2]);
                    indices.push_back(face[count -1]);
                }
                
                next= next +n;
                if(n == 0 || count >= 4)
                    break;
            }
        }
    }
    
    fclose(in);
    if(error)
    {
        printf("loading mesh '%s'... failed.\n  %lu/%lu/%lu triangles, parsing line:\n%s\n", 
            filename.c_str(), positions.size(), texcoords.size(), normals.size(),
            line);
        delete mesh;
        return NULL;
    }
    
    if(indices.empty())
    {
        // renvoie les donnes brutes, sans essayer de construire une indexation lineaire.
        mesh->positions.swap(positions);
        
        printf("  %lu positions, %lu texcoords, %lu normals\n",  
            mesh->positions.size(), mesh->texcoords.size(), mesh->normals.size());
        printf("done.\n");
        return mesh;
    }
    
    // cree une matiere par defaut, si necessaire
    if(has_material == false)
    {
        materials.push_back( MeshMaterial() );  
        
        // affecte la matiere par defaut
        for(unsigned int i= 0; i < indices.size(); i++)
            if(indices[i].material < 0)
                indices[i].material= materials.size() -1; 
    }
    
    // construit l'index buffer avec indexation unique
    // et re ordonne les attributs
    std::map<OBJ::index, int> remap;
    mesh->indices.reserve(indices.size());
    
    // identifie les triplets uniques d'attributs pour construire une indexation "unique" / vertex buffer lineaire.
    material_id= -1;
    for(unsigned int i= 0; i < indices.size(); i++)
    {
        std::pair< std::map<OBJ::index, int>::iterator, bool > found= 
            remap.insert( std::make_pair(indices[i], remap.size()) );
        mesh->indices.push_back(found.first->second);
        
        // cree les groupes de faces utilisant la meme matiere
        //! \todo fusionner les groupes de faces utilisant la meme matiere.
        if(found.first->first.material != material_id)
        {
            material_id= found.first->first.material;
            mesh->groups.push_back( MeshGroup(materials[material_id], i) );
        }
        
        mesh->groups.back().end= i +1;
    }
    
    //~ for(unsigned int i= 0; i < mesh->groups.size(); i++)
        //~ printf("group %s, begin %d end %d\n  diffuse '%s'\n  specular '%s'\n", 
            //~ mesh->groups[i].material.name.c_str(), mesh->groups[i].begin /3, mesh->groups[i].end /3,
            //~ mesh->groups[i].material.diffuse_texture.c_str(), mesh->groups[i].material.specular_texture.c_str());
    
    // affecte les matieres aux triangles 
    mesh->materials.resize(mesh->indices.size() / 3u, -1);
    for(unsigned int g= 0; g < mesh->groups.size(); g++)
    {
        unsigned int begin= mesh->groups[g].begin / 3u;
        unsigned int end= mesh->groups[g].end / 3u;
        
        for(unsigned int i= begin; i < end; i++)
            mesh->materials[i]= g;
    }
    
    // pre-alloue les buffers
    mesh->positions.resize(remap.size());
    if(has_texcoord) mesh->texcoords.resize(remap.size());
    if(has_normal) mesh->normals.resize(remap.size());
    
    // reordonne les attributs
    for(unsigned int i= 0; i < indices.size(); i++)
    {
        const int index= mesh->indices[i];
        if(indices[i].position == -1)
        {
            printf("loading mesh '%s'... failed.\n  invalid structure.\n", filename.c_str());
            delete mesh;
            return NULL;
        }
        
        mesh->positions[index]= positions[indices[i].position];
        
        if(has_texcoord && indices[i].texcoord != -1)
            mesh->texcoords[index]= texcoords[indices[i].texcoord];
        
        if(has_normal && indices[i].normal != -1)
            mesh->normals[index]= normals[indices[i].normal];
    }
    
    printf("  %lu positions, %lu texcoords, %lu normals, %lu triangles\n",  
        mesh->positions.size(), mesh->texcoords.size(), mesh->normals.size(), 
        mesh->indices.size() / 3);
    printf("  %lu material groups\n", mesh->groups.size());
    printf("done.\n");
    return mesh;
}


//! analyse un fichier .mtl et construit un ensemble de description de matieres.
int readMTL( const std::string& filename, std::vector<MeshMaterial>& materials )
{
    FILE *in= fopen(filename.c_str(), "rt");
    if(in == NULL)
    {
        printf("error reading '%s'.\n", filename.c_str());
        return -1;
    }
    
    printf("loading '%s'...\n", filename.c_str());
    
    char line[1024];
    char tmp[1024];
    MeshMaterial *material= NULL;
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(fgets(line, sizeof(line), in) == NULL)
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        line[sizeof(line) -1]= 0;        // force la fin de la ligne, a cas ou
        if(line[0] == 'n')
        {
            if(sscanf(line, "newmtl %[^\r\n]", tmp) == 1)
            {
                materials.push_back( MeshMaterial(tmp) );
                material= &materials.back();
            }
        }
        if(material == NULL)
            continue;
        
        if(line[0] == 'K')
        {
            float r, g, b;
            if(sscanf(line, "Kd %f %f %f", &r, &g, &b) == 3) 
            {
                Vector v(r, g, b);
                float k= v.Length();
                if(k > 0) v= Normalize(v);
                material->diffuse_color= VecColor(v.x, v.y, v.z, 1);
                material->kd= k;
            }
            else if(sscanf(line, "Ks %f %f %f", &r, &g, &b) == 3) 
            {
                Vector v(r, g, b);
                float k= v.Length();
                if(k > 0) v= Normalize(v);
                material->specular_color= VecColor(v.x, v.y, v.z, 1);
                material->ks= k;
            }
        }
        
        if(line[0] == 'k')
        {
            float k;
            if(sscanf(line, "kd %f", &k) == 1)
                material->kd= k;
            else if(sscanf(line, "ks %f", &k) == 1)
                material->ks= k;
        }
        
        else if(line[0] == 'm')
        {
            if(sscanf(line, "map_Kd %[^\r\n]", tmp) == 1)
                material->diffuse_texture= IOFileSystem::pathname( filename ) + tmp;
            else if(sscanf(line, "map_Ks %[^\r\n]", tmp) == 1)
                material->specular_texture= IOFileSystem::pathname( filename ) + tmp;
        }
        
        else if(line[0] == 'N') // Ni Ns, indice de refraction pour les miroirs et les objets transparents
        {
            float n;
            if(sscanf(line, "Ns %f", &n) == 1)
                material->ns= n;
            else if(sscanf(line, "Ni %f", &n) == 1)
                material->ni= n;
        }
        
        else if(line[0] == 'L')     // Le
        {
            float r, g, b;
            if(sscanf(line, "Le %f %f %f", &r, &g, &b) == 3) 
                material->emission= VecColor(r, g, b);
        }
    }
    
    fclose(in);
    if(error)
    {
        printf("loading '%s'... failed, parsing line :\n%s\n", filename.c_str(), line);
        return -1;
    }
    
    return 0;
}


//! calcule les normales moyenne par sommet.
int buildNormals( Mesh *mesh )
{
    if(mesh == NULL)
        return -1;

    mesh->normals.assign(mesh->positions.size(), Vec3());
    
    const unsigned int triangles= mesh->indices.size() / 3u;
    for(unsigned int i= 0; i < triangles; i++)
    {
        const unsigned int ia= mesh->indices[3u*i];
        const unsigned int ib= mesh->indices[3u*i +1u];
        const unsigned int ic= mesh->indices[3u*i +2u];
        
        const Point a(mesh->positions[ia]);
        const Point b(mesh->positions[ib]);
        const Point c(mesh->positions[ic]);
        
        // attention aux triangles degeneres avec des aretes de longueur nulles
        Vector ab(a, b);
        //~ if(ab.Length() > 0.0f)
            //~ ab= Normalize(ab);
        
        Vector ac(a, c);
        //~ if(ac.Length() > 0.0f)
            //~ ac= Normalize(ac);
        
        //~ Vector bc(b, c);
        //~ if(bc.Length() > 0.0f)
            //~ bc= Normalize(bc);
        
        Vector n= Cross(ab, ac);
        mesh->normals[ia]= Vector(mesh->normals[ia]) + n;
        mesh->normals[ib]= Vector(mesh->normals[ib]) + n;
        mesh->normals[ic]= Vector(mesh->normals[ic]) + n;
    }
    
    for(unsigned int i= 0; i < mesh->normals.size(); i++)
    {
        Vector v(mesh->normals[i]);
        //~ if(v.Length() > 0.0f)
            mesh->normals[i]= Normalize(v);
    }
    
    return 0;
}

}       // namespace

}       // namespace

