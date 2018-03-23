#include "viewerMesh.h"

ViewerMesh::ViewerMesh()
{
    pMesh = new CMesh();
}

ViewerMesh::~ViewerMesh()
{

}

int ViewerMesh::input_obj(std::string fname)
{
    std::fstream f(fname, std::fstream::in);
    if (f.fail()) return 3;

    std::string cmd;
    int  vid = 1;
    int  fid = 1;

    bool with_uv = false;
    bool with_normal = false;

    std::vector<CPoint2> uvs;
    std::vector<CPoint> normals;

    while (std::getline(f, cmd))
    {
        std::string line_info(cmd);
        if (line_info.empty()) continue;
        if (line_info[0] == '\0')
        {
            continue;
        }
        char * token;
        token = strtok(&cmd[0], " \t\r\n");

        std::string check_token(token);


        if (check_token == "v")
        {
            token = strtok(NULL, " \t\r\n");
            CPoint p;
            int i = 0;
            while (token != nullptr)
            {
                if (i > 2)
                {
                    break;
                }
                p[i] = atof(token);
                i++;
                token = strtok(NULL, " \t\r\n");
            }

            CVertex * v = m_mesh()->create_vertex(vid);
            v->point() = p;
            vid++;
            continue;
        }


        if (check_token == "vt")
        {
            with_uv = true;
            mesh_with_uv = true;
            CPoint2 uv;
            token = strtok(NULL, " \t\r\n");
            int i = 0;
            while (token != nullptr)
            {
                if (i > 1)
                {
                    break;
                }
                uv[i] = atof(token);
                i++;
                token = strtok(NULL, " \t\r\n");
            }
            uvs.push_back(uv);
            continue;
        }

        if (check_token == "vn")
        {
            with_normal = true;
            mesh_with_normal = true;
            token = strtok(NULL, " \t\r\n");
            CPoint n;
            int i = 0;
            while (token != nullptr)
            {

                n[i] = atof(token);
                i++;
                token = strtok(NULL, " \t\r\n");
            }
            normals.push_back(n);
            continue;
        }

        if (check_token == "f")
        {
            std::vector<CVertex*> vs;
            token = strtok(NULL, " /\t\r\n");
            while (token != nullptr)
            {
                int ids[3];
                int k = 0;
                int loop_count = 1 + with_uv + with_normal;
                for (int j = 0; j<loop_count; j++)
                {
                    ids[k] = atoi(token);
                    k++;
                    token = strtok(NULL, " /\t\r\n");
                }

                CVertex * vi = m_mesh()->m_map_vert[ids[0]];
                if (with_uv)
                    vi->uv() = uvs[ids[1] - 1];
                if (with_normal && !with_uv)
                {
                    vi->normal() = normals[ids[1] - 1];
                }
                if (with_normal && with_uv)
                {
                    vi->normal() = normals[ids[2] - 1];
                }
                vs.push_back(vi);
            }
            if (vs.size() == 3)
            {
                CFace * c_pf = m_mesh()->create_face(vs, fid++);
            }
            else if (vs.size() == 4)
            {
                // loop the vertex vector to decompose polygon to triangles.
                for (int i_if = 0; i_if < vs.size() - 2; i_if++)
                {
                    std::vector<CVertex *> vs_c;
                    vs_c.push_back(vs[0]);
                    vs_c.push_back(vs[i_if + 1]);
                    vs_c.push_back(vs[i_if + 2]);
                    CFace * c_pf = m_mesh()->create_face(vs_c, fid++);
                }
            }
            else
            {
                std::cout << "Polygons with more than four edges are not supported." << std::endl;
                exit(-3);
            }
            continue;
        }

        if (check_token == "mtllib")
        {
            std::string tx_info_i;
            while (token != nullptr)
            {
                tx_info_i += token;
                tx_info_i += " ";
                token = strtok(NULL, " \n");
            }
            tx_info_i = tx_info_i.substr(0, tx_info_i.size() - 1);
            continue;
        }
        if (check_token == "#")
        {
            continue;
        }
        else
        {
            continue;
        }
    }

    if (normalize())
    {
        std::cout << "Normalizaion Issue" << std::endl;
        return 2;
    }

    f.close();
    m_mesh()->label_boundary();

    return 0;
}

int ViewerMesh::normalize()
{
    double x_min = DBL_MAX, y_min = DBL_MAX, z_min = DBL_MAX;
    double x_max = DBL_MIN, y_max = DBL_MIN, z_max = DBL_MIN;
    CPoint cp_min;
    CPoint cp_a(0,0,0);
    double x_d, y_d, z_d;

    for (CVertex * pv : m_mesh()->vertices())
    {
        CPoint cp = pv->point();
        for (int i = 0; i < 3; i++)
        {
            cp_a[i] += cp[i];
        }
        x_min = std::min(x_min, cp[0]);
        x_max = std::max(x_max, cp[0]);
        y_min = std::min(y_min, cp[1]);
        y_max = std::max(y_max, cp[1]);
        z_min = std::min(z_min, cp[2]);
        z_max = std::max(z_max, cp[2]);
    }
    cp_a /= m_mesh()->vertices().size();
    cp_min = CPoint(x_min, y_min, z_min);
    x_d = x_max - x_min;
    y_d = y_max - y_min;
    z_d = z_max - z_min;
    if (x_d < EPS || y_d < EPS || z_d < EPS)
    {
        std::cout << "Normalization Issue. May caused because object is too small" << std::endl;
        return 2;
    }
    double diff = std::max(x_d, std::max(y_d, z_d));
    for (CVertex * pv : m_mesh()->vertices())
    {
        CPoint cp = pv->point()-cp_a;
        CPoint cp_new;
        cp_new[0] = 2 * cp[0] / diff;
        cp_new[1] = 2 * cp[1] / diff;
        cp_new[2] = 2 * cp[2] / diff;
        pv->point() = cp_new;
    }
    return 0;
}