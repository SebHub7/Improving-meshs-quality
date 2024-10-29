#include "program.h"
#include "app.h"
#include <./tutos/Mesh2.h>
#include <orbiter.h>
#include <uniforms.h>
#include <./tutos/Vector3.h>
#include <chrono>


class TP : public App
{
public:
    TP() : App(800, 600) {}

    Vector3 getNormal(index face)
    {
        Face* f = &m.faces[face];
        Vector3 s0(m.vertices[f->vertices[0]]);
        Vector3 s1(m.vertices[f->vertices[1]]);
        Vector3 s2(m.vertices[f->vertices[2]]);

        Vector3 v1(s1.x - s0.x, s1.y - s0.y, s1.z - s0.z);
        Vector3 v2(s2.x - s0.x, s2.y - s0.y, s2.z - s0.z);

        v1.normalize();
        v2.normalize();

        Vector3 n = Vector3::cross(v1, v2);
        return n;
    }

    void updateMeshBuffer()
    {
        vertices.clear();
        printf("Vertices size: %d\n", m.vertices.size());
        printf("Faces size: %d\n", m.faces.size());

        for (index i = 0; i < m.faces.size(); i++)
        {
            Face& f = m.faces[i];

            if (!m.isTriangleInfinite(i))
            {
                vertices.push_back(Vector3(m.vertices[f.vertices[0]]));
                vertices.push_back(Vector3(m.vertices[f.vertices[1]]));
                vertices.push_back(Vector3(m.vertices[f.vertices[2]]));

                normals.push_back(getNormal(i));
                normals.push_back(getNormal(i));
                normals.push_back(getNormal(i));
            }
        }

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3), &vertices.front().x, GL_STATIC_DRAW);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vector3), &normals.front().x, GL_STATIC_DRAW);
    }
    
    void cameraControl()
    {
        int mx, my;
        unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
        int mousex, mousey;
        SDL_GetMouseState(&mousex, &mousey);

        if (mb & SDL_BUTTON(1))
            camera.rotation(mx, my);
        else if (mb & SDL_BUTTON(3))
            camera.translation((float)mx / (float)window_width(), (float)my / (float)window_height());
        else if (mb & SDL_BUTTON(2))
            camera.move(mx);

        SDL_MouseWheelEvent wheel = wheel_event();
        if (wheel.y != 0)
        {
            clear_wheel_event();
            camera.move(8.f * wheel.y);
        }
    }

    int init()
    {
        srand(time(NULL));
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // vertices
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);

        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // normals
        glGenBuffers(1, &vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);

        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);

        m_program = read_program("tutos/tuto9_color.glsl");
        program_print_errors(m_program);
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        
        glClearDepth(1.f);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);

        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        vertices_file = m.readPointCloudFile("data/alpes_poisson.txt");
        printf("\nProgram start inserting...\n");

        for (Sommet& s : vertices_file)
        {
            Sommet temp = s;
            temp.z = 0;
            m.insertPoint(temp);
        }

        for (int i = 0; i < vertices_file.size(); i++)
            m.vertices[i+1].z = vertices_file[i].z;

        updateMeshBuffer();

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        unsigned int time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        printf("Insertions done, %d ms\n", time);

        camera.projection(window_width(), window_height(), 45);
        Sommet min(999999, 999999, 999999);
        Sommet max(-999999, -999999, -999999);
        m.getMeshBounds(vertices_file, min, max);
        camera.lookat(Point(min.x, min.y, min.z), Point(max.x, max.y, max.z));

        return 0;
    }
    
    int quit()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo_vertices);
        release_program(m_program);
        return 0;
    }
    
    int render()
    {
        cameraControl();

        if (added_point)
        {
            /*added_point = false;
            float x = ((float)mousex / 400) - 1;
            float y = ((float)mousey / 300) - 1;
            Sommet s(x, -y, 0);
            
            Sommet s1 = test[cpt_vertices_inserted];
            s1.z = 0;
            m.insertPoint(s1);
            cpt_vertices_inserted++;

            if (m.faces.size() >= 1)
                updateMeshBuffer();*/
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(m_program);
        glBindVertexArray(vao);

        Transform view = camera.view();
        Transform projection = camera.projection();
        Transform vp = projection * view;

        program_uniform(m_program, "vpMatrix", vp);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        return 1;
    }

protected:
    
    Mesh2 m;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;

    Orbiter camera;
    GLuint vao;
    GLuint vbo_vertices;
    GLuint vbo_normals;
    GLuint m_program;

    std::vector<Sommet> vertices_file;
    int cpt_vertices_inserted = 0;

    std::vector<Sommet> test;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();

    return 0;
}