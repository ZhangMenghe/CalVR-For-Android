#include "dcmRenderer.h"
#include "Color.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <set>
using namespace glm;
template <typename T>
std::set<T> getUnion(const std::set<T>& a, const std::set<T>& b)
{
    std::set<T> result = a;
    result.insert(b.begin(), b.end());
    return result;
}
bool getRayIntersection(vec3 ray_start, vec3 ray_dir, vec3 plane_point, vec3 plane_norm, vec3& out_point){
    float ray_plane_angle  = dot(ray_dir, plane_norm);
    if(ray_plane_angle == .0f)
        return false;
    //AX + BY + CZ + D = 0
    //D = -(PLANE_POINT * PLANE_NORM)
    //N = RAY_DIR * PLANE_NORM
    out_point =  ray_start - (( dot(plane_norm, ray_start) - dot(plane_point, plane_norm)) / ray_plane_angle) * ray_dir;
    return true;
}

vec3 getPlaneNormal(vec3 a, vec3 b, vec3 c){
    return normalize(cross(b-a, c-a));
}
bool vecComp(const vec3& v1, const vec3& v2){
    if(v1.x != v2.x) return v1.x< v2.x;

    if(v1.y != v2.y ) return v1.y < v2.y;

    return v1.z<v2.z;
}
vec3 findMinVec3(std::vector<vec3> points){
    auto res = std::min_element(points.begin(), points.end(), vecComp);
    return *res;
}

/************************************
 * getIntersectionPolygon: get all the points that form the polygon of plane-aabb intersection.
 * points starting from minimal x then y then z value
 * @param p point on plane, p_normal:surface normal of plane
 * @param aabb_min
 * @param aabb_max
 * @param polygon
 */
void getIntersectionPolygon(vec3 p, vec3 p_norm, vec3 aabb_min, vec3 aabb_max, std::vector<Polygon>& polygon, PolygonMap& face_map){
    vec3 dir; // direction to test
    vec3 p_rp;//current answer
    int pid = 0;
    // Test edges along X axis, pointing right.
    dir = vec3(aabb_max.x - aabb_min.x, .0f, .0f);
    if(getRayIntersection(aabb_min, dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BACK].push_back(pid);
        face_map[BOTTOM].push_back(pid);
        pid++;
    }

    if(getRayIntersection(vec3(aabb_min.x, aabb_max.y, aabb_min.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BACK].push_back(pid);
        face_map[UP].push_back(pid);
        pid++;
    }
    if(getRayIntersection(vec3(aabb_min.x, aabb_min.y, aabb_max.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[FRONT].push_back(pid);
        face_map[BOTTOM].push_back(pid);
        pid++;
    }

    if(getRayIntersection(vec3(aabb_min.x, aabb_max.y, aabb_max.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[FRONT].push_back(pid);
        face_map[UP].push_back(pid);
        pid++;
}

    // Test edges along Y axis, pointing up.
    dir = vec3(0.f, aabb_max.y - aabb_min.y, 0.f);
    if(getRayIntersection(vec3(aabb_min.x, aabb_min.y, aabb_min.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BACK].push_back(pid);
        face_map[LEFT].push_back(pid);
        pid++;
    }
    if(getRayIntersection(vec3(aabb_max.x, aabb_min.y, aabb_min.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BACK].push_back(pid);
        face_map[RIGHT].push_back(pid);
        pid++;
    }
    if(getRayIntersection(vec3(aabb_min.x, aabb_min.y, aabb_max.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[FRONT].push_back(pid);
        face_map[LEFT].push_back(pid);
        pid++;
    }
    if(getRayIntersection(vec3(aabb_max.x, aabb_min.y, aabb_max.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[FRONT].push_back(pid);
        face_map[RIGHT].push_back(pid);
        pid++;
    }

    // Test edges along Z axis, pointing forward.
    dir = vec3(0.f, 0.f, aabb_max.z - aabb_min.z);
    if(getRayIntersection(vec3(aabb_min.x, aabb_min.y, aabb_min.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[LEFT].push_back(pid);
        face_map[BOTTOM].push_back(pid);
        pid++;
    }
    if(getRayIntersection(vec3(aabb_max.x, aabb_min.y, aabb_min.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BOTTOM].push_back(pid);
        face_map[RIGHT].push_back(pid);
        pid++;
    }
    if(getRayIntersection(vec3(aabb_min.x, aabb_max.y, aabb_min.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[UP].push_back(pid);
        face_map[LEFT].push_back(pid);
        pid++;
    }
    if(getRayIntersection(vec3(aabb_max.x, aabb_max.y, aabb_min.z), dir, p, p_norm, p_rp)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[UP].push_back(pid);
        face_map[RIGHT].push_back(pid);
        pid++;
    }

    //sort the plane
    if(polygon.empty()) return;

    //left-bottom corner
    vec3 origin = polygon[0].first;//findMinVec3(polygon);//polygon[0];

    std::sort(polygon.begin(), polygon.end(), [&](const Polygon& p1, const Polygon& p2)->bool{
        vec3 cross_v = glm::cross(p1.first - origin, p2.first - origin);
        return dot(cross_v, p_norm)>0;
    });
}


dcmVolumeRender::dcmVolumeRender(AAssetManager *assetManager):
        cubeRenderer(assetManager){
    new assetLoader(assetManager);
//    _modelMat = glm::scale(_modelMat, glm::vec3(1,1,0.3f));
}

void dcmVolumeRender::addImage(GLubyte * img, float location) {
    images_.push_back(new dcmImage(
            img,
            location));
}
void dcmVolumeRender::setting_1D_texture(){
//    GLuint textureID;
    //enable texture unit
    glActiveTexture(GL_TEXTURE1);
    //create texture object
    glGenTextures(1, &trans_texid);
    glBindTexture(GL_TEXTURE_2D, trans_texid);
    //bind current texture object and set the data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, transfer_color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
void dcmVolumeRender::assembleTexture() {
    std::sort(images_.begin(), images_.end(),
              [](const dcmImage* img1, const dcmImage* img2){return img1->location < img2->location;});
    dimensions = images_.size();

    size_t data_size = img_width * img_height * dimensions;
    auto *data = new GLubyte[data_size];
    auto each_size = img_height * img_width* UI_SIZE;
    for(int i=0; i<dimensions; i++)
        memcpy(data+i*each_size, images_[i]->data, each_size);

    glGenTextures(1, &volume_texid);
    // bind 3D texture target
    glBindTexture(GL_TEXTURE_3D, volume_texid);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // pixel transfer happens here from client to OpenGL server
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, (int)img_width, (int)img_height, (int)dimensions, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    delete[]data;

    setting_1D_texture();

    initGeometry_texturebased();

    initGeometry();
}
void dcmVolumeRender::initGeometry_Naive() {
    float vertices[] = {
            -0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,0.0f,0.0f,
            0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,1.0f,0.0f,
            0.5f,0.5f,-0.5f,0.0f,0.0f,-1.0f,1.0f,1.0f,
            0.5f,0.5f,-0.5f,0.0f,0.0f,-1.0f,1.0f,1.0f,
            -0.5f,0.5f,-0.5f,0.0f,0.0f,-1.0f,0.0f,1.0f,
            -0.5f,-0.5f,-0.5f,0.0f,0.0f,-1.0f,0.0f,0.0f,
            -0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,0.0f,0.0f,
            0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,1.0f,0.0f,
            0.5f,0.5f,0.5f,0.0f,0.0f,1.0f,1.0f,1.0f,
            0.5f,0.5f,0.5f,0.0f,0.0f,1.0f,1.0f,1.0f,
            -0.5f,0.5f,0.5f,0.0f,0.0f,1.0f,0.0f,1.0f,
            -0.5f,-0.5f,0.5f,0.0f,0.0f,1.0f,0.0f,0.0f,
            -0.5f,0.5f,0.5f,-1.0f,0.0f,0.0f,1.0f,0.0f,
            -0.5f,0.5f,-0.5f,-1.0f,0.0f,0.0f,1.0f,1.0f,
            -0.5f,-0.5f,-0.5f,-1.0f,0.0f,0.0f,0.0f,1.0f,
            -0.5f,-0.5f,-0.5f,-1.0f,0.0f,0.0f,0.0f,1.0f,
            -0.5f,-0.5f,0.5f,-1.0f,0.0f,0.0f,0.0f,0.0f,
            -0.5f,0.5f,0.5f,-1.0f,0.0f,0.0f,1.0f,0.0f,
            0.5f,0.5f,0.5f,1.0f,0.0f,0.0f,1.0f,0.0f,
            0.5f,0.5f,-0.5f,1.0f,0.0f,0.0f,1.0f,1.0f,
            0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f,0.0f,1.0f,
            0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f,0.0f,1.0f,
            0.5f,-0.5f,0.5f,1.0f,0.0f,0.0f,0.0f,0.0f,
            0.5f,0.5f,0.5f,1.0f,0.0f,0.0f,1.0f,0.0f,
            -0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,1.0f,
            0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,1.0f,1.0f,
            0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,1.0f,0.0f,
            0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,1.0f,0.0f,
            -0.5f,-0.5f,0.5f,0.0f,-1.0f,0.0f,0.0f,0.0f,
            -0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f,0.0f,1.0f,
            -0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f,0.0f,1.0f,
            0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f,1.0f,1.0f,
            0.5f,0.5f,0.5f,0.0f,1.0f,0.0f,1.0f,0.0f,
            0.5f,0.5f,0.5f,0.0f,1.0f,0.0f,1.0f,0.0f,
            -0.5f,0.5f,0.5f,0.0f,1.0f,0.0f,0.0f,0.0f,
            -0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f,0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO[0]);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    mProgram = assetLoader::instance()->createGLShaderProgramFromFile("shaders/cube.vert", "shaders/cube.frag");
    if(!mProgram)
        LOGE("===Failed to create shader program===");
    glClearColor(0,0,0,0);
}
void dcmVolumeRender::onViewCreated(){
    program_ray = assetLoader::instance()->createGLShaderProgramFromFile("shaders/raycastVolume.vert", "shaders/raycastVolume.frag");
    program_plane = assetLoader::instance()->createGLShaderProgramFromFile("shaders/IntersectionPlane.vert", "shaders/IntersectionPlane.frag");

    program_texture = assetLoader::instance()->createGLShaderProgramFromFile("shaders/textureVolume.vert", "shaders/textureVolume.frag");
    if(!program_ray || !program_texture || !program_plane)
        LOGE("===Failed to create shader program===");
}
void dcmVolumeRender::setZpos(float nz){

}
//update vertices, indices, and corresponding numbers
void dcmVolumeRender::updateGeometry(std::vector<Polygon> polygon, PolygonMap polygon_map, std::vector<int> rpoints){
    delete(vertices_);
    delete(indices_);
    //create new vertices
    int rp_num = rpoints.size();
    vertices_num_ = rp_num + polygon.size();
    std::vector<int>id_map(vertices_num_, 0);
//    int id_map[vertices_num_] = {0};

    vertices_ = new GLfloat[3*vertices_num_];
    int nidx = 0;
    for(auto id:rpoints){
        vertices_[3*nidx] = sVertex[id*3];vertices_[3*nidx+1] = sVertex[id*3+1];vertices_[3*nidx+2] = sVertex[id*3+2];
        id_map[nidx] = id; nidx++;
    }
    for(auto point:polygon){
        vertices_[3*nidx] = point.first.x;vertices_[3*nidx+1] = point.first.y;vertices_[3*nidx+2] = point.first.z;
        id_map[nidx] = point.second; nidx++;
    }
    //points are copied
    //faces

    std::vector<GLuint> c_indices;
    std::set<int>rp_set(rpoints.begin(), rpoints.end());
    for(int i=0; i<6; i++){
//        auto uset = getIntersection(rp_set, );
        std::set<int> uset;
        std::set<int> face_set(sIndices+i*6, sIndices+(i+1)*6);
        set_intersection(rp_set.begin(),rp_set.end(),face_set.begin(),face_set.end(),
                         std::inserter(uset,uset.begin()));

        if(uset.empty()) continue;
        auto got = polygon_map.find((Face)i);
        if(got == polygon_map.end()) {
            //c_indices.insert(c_indices.end(),sIndices+i*6,sIndices+(i+1)*6 );
            int start_pos = i*6, cid;
            for(int j=0; j< 6;j++){
                cid = std::distance(id_map.begin(), std::find(id_map.begin(), id_map.begin()+rp_num, sIndices[start_pos+j]));
                c_indices.push_back(cid);
            }
        }
        else{
            //re order the points
            //for new points: get id -> vec3
            std::vector<Polygon>face_points;
            for(auto id:got->second){
                int cid = std::distance(id_map.begin(), std::find(id_map.begin()+rp_num, id_map.end(), id));
                face_points.push_back(std::make_pair(vec3(vertices_[3*cid], vertices_[3*cid+1], vertices_[3*cid+2]), cid));
            }
            for(auto id:uset){
                int cid = std::distance(id_map.begin(), std::find(id_map.begin(), id_map.begin()+rp_num, id));
                face_points.push_back(std::make_pair(vec3(vertices_[3*cid], vertices_[3*cid+1], vertices_[3*cid+2]), cid));
            }

            vec3 origin = face_points[0].first;
            vec3 p_norm = getPlaneNormal(face_points[0].first, face_points[1].first, face_points[2].first);
            std::sort(face_points.begin(), face_points.end(), [&](const Polygon& p1, const Polygon& p2)->bool{
                vec3 cross_v = glm::cross(p1.first - origin, p2.first - origin);
                return dot(cross_v, p_norm)>0;
            });


            //face points are sorted counter_clock_wise
            for(int i=0; i<face_points.size()-2; i++){
                GLuint indice[] = {(GLuint)face_points[0].second, (GLuint)face_points[i+1].second, (GLuint)face_points[i+2].second};
                c_indices.insert(c_indices.end(), indice, indice+3);
            }
        }
    }

    int npid[polygon.size()];
    for(int i=0;i<polygon.size();i++)
        npid[i] = std::distance(id_map.begin(), std::find(id_map.begin()+rp_num, id_map.end(), polygon[i].second));
    //generate face that all from new points'
    for(int i=0; i<polygon.size()-2; i++){
        GLuint indice[] = {(GLuint)npid[0], (GLuint)npid[i+1], (GLuint)npid[i+2]};
        c_indices.insert(c_indices.end(), indice, indice+3);
    }

    indices_ = c_indices.data();
    indices_num_ = c_indices.size();
}
void dcmVolumeRender::updateVBOData(){

}
void dcmVolumeRender::initGeometry() {
    stepsize_ = vec3(1.0f / img_width, 1.0f / img_height, 1.0f/dimensions);
    indices_num_ = 36;
    vertices_num_ = 8;

    vertices_ = new GLfloat[vertices_num_ * 3];
    indices_ = new GLuint[indices_num_];
    memcpy(vertices_, sVertex, sizeof(GLfloat) * 3 * vertices_num_);
    memcpy(indices_, sIndices, sizeof(GLuint) * indices_num_);
////////////////////////////////////////////////////
    //debug: setup clip plane
    cplane_points_num_ = 4;
    cplane_points_ = new float[12];
    for(int i=0;i<4;i++){
        cplane_points_[3*i] = sVertex[3*i]*1.5f ;
        cplane_points_[3*i+1] = sVertex[3*i+1]*1.5f;
        cplane_points_[3*i+2] = .0f;
    }

    std::vector<vec3> plane_points;
    polygon.clear();
    for(int i=0; i<cplane_points_num_; i++)
        plane_points.push_back(vec3(cplane_points_[3*i], cplane_points_[3*i+1],cplane_points_[3*i+2]));
    vec3 p_norm = getPlaneNormal(plane_points[0], plane_points[1], plane_points[2]);

    //view_dir and p_norm should be on the same side
    if(dot(p_norm, _camera->getViewDirection()) < 0)
        p_norm = -p_norm;
    getIntersectionPolygon(plane_points[0], p_norm, vec3(-0.5f), vec3(0.5f), polygon, polygon_map);

    for(int i=0; i<polygon.size();i++)
        LOGE("====INTERSECTION id : %d, pos:  %f, %f, %f\n", polygon[i].second, polygon[i].first.x, polygon[i].first.y,polygon[i].first.z);

    for(auto item : polygon_map){
        LOGE("====== face %d", item.first);
        for(auto id : item.second){
            LOGE("===id: %d", id);
        }
        LOGE("====\n");
    }
    //Test 8 points which should be remove
    std::vector<int> rpoints;

    for(int i=0;i<8;i++){
        vec3 vertex = vec3(sVertex[3*i], sVertex[3*i+1],sVertex[3*i+2]);//point to test
        vec3 tdir = vertex- plane_points[0];
        if(dot(tdir, p_norm) >= 0)
            rpoints.push_back(i);
    }

    for(int i=0; i<rpoints.size(); i++) LOGE("====keep point: %d", rpoints[i]);

    // rpoint are in accending order
    //for each face, create new vertices and indices
    updateGeometry(polygon, polygon_map, rpoints);
    //now vertices and Indices are updated

    //insert plane-points back to cube

//    int vertex_num, indice_num;
//    updateGeometry(Vertex, Indices, vertex_num, indice_num);




    /*GLuint Indices_plane[] = {0,1,2,0,2,3};

    glGenVertexArrays(1, &VAO_PLANE);
    glBindVertexArray(VAO_PLANE);

    glGenBuffers(1, &VBO_PLANE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PLANE);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * cplane_points_num_, cplane_points_, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

//    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, nullptr, GL_DYNAMIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint EBO_PLANE;
    glGenBuffers(1, &EBO_PLANE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_PLANE);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GL_UNSIGNED_INT), Indices_plane, GL_STATIC_DRAW);

    //Unbind the vertex array
    glBindVertexArray(0);

    //Disable Buffers and vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/

////////////////////////////////////////////////////


    glGenBuffers(1, VBO);
    glGenBuffers(1, &EBO);

    // bind buffer for positions and copy data into buffer
    // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // feed the buffer, and let OpenGL know that we don't plan to
    // change it (STATIC) and that it will be used for drawing (DRAW)
    glBufferData(GL_ARRAY_BUFFER, 3*vertices_num_* sizeof(GL_FLOAT), vertices_, GL_STATIC_DRAW);

    //Set the index array
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_num_ * sizeof(GL_UNSIGNED_INT), indices_, GL_STATIC_DRAW);

    //Generate the VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // bind buffer for positions and copy data into buffer
    // GL_ARRAY_BUFFER is the buffer type we use to feed attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);

    //Unbind the vertex array
    glBindVertexArray(0);

    //Disable Buffers and vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_TEXTURE_3D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    delete(vertices_);
    delete(indices_);
}

void dcmVolumeRender::initGeometry_texturebased() {
    m_VAOs = new GLuint[dimensions];
    float dimension_inv = 1.0f / dimensions;

    for (int i = 0; i < dimensions; i++)
    {
        float mappedZVal = (-1.0f + 2.0f * (float)i * dimension_inv)*scale_inv;

//        float zTex = (float)mappedZVal + 0.5f;
        float zTex = (float)i * dimension_inv;
        float vertices[] = {
                // positions		// texture coords
                -0.5f,  -0.5f, mappedZVal,	0.0f, 0.0f, zTex,   // top right
                0.5f, -0.5f, mappedZVal,	1.0f, 0.0f, zTex,  // bottom right
                0.5f, 0.5f, mappedZVal,		1.0f, 1.0f, zTex,  // bottom left
                -0.5f,  0.5f, mappedZVal,	0.0f, 1.0f,  zTex  // top left
        };

        unsigned int VBO, EBO;
        unsigned int indices[] = {
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &m_VAOs[i]);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray((GLuint)m_VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*6, indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
}
void dcmVolumeRender::onDraw() {
    switch(render_mode){
        case TEXTURE_BASED:
            onTexturebasedDraw();
            break;
        case RAYCAST:
            onRaycastDraw();
            break;
        default:
            onTexturebasedDraw();
            break;
    }
}
void dcmVolumeRender::onNaiveDraw() {
//    glClear(GL_COLOR_BUFFER_BIT);
//    glUseProgram(mProgram);
//    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,VERTEX);
//    glEnableVertexAttribArray(0);
//    glDrawArrays(GL_TRIANGLES,0,3);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "uProjMat"), 1, GL_FALSE, &(_camera->getProjMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "uViewMat"), 1, GL_FALSE, &(_camera->getViewMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(mProgram, "uModelMat"), 1, GL_FALSE, &_modelMat[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
void dcmVolumeRender::onTexturebasedDraw(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < dimensions; i++) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, volume_texid);

        glUseProgram(program_texture);
        glUniformMatrix4fv(glGetUniformLocation(program_texture, "uProjMat"), 1, GL_FALSE,
                           &(_camera->getProjMat()[0][0]));
        glUniformMatrix4fv(glGetUniformLocation(program_texture, "uViewMat"), 1, GL_FALSE,
                           &(_camera->getViewMat()[0][0]));
        glUniformMatrix4fv(glGetUniformLocation(program_texture, "uModelMat"), 1, GL_FALSE,
                           &_modelMat[0][0]);

        glUniform1i(glGetUniformLocation(program_texture, "uSampler_tex"), 0);
        glBindVertexArray(m_VAOs[i]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}
void dcmVolumeRender::onRaycastDraw(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, volume_texid);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, trans_texid);

    glUseProgram(program_ray);

    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uProjMat"), 1, GL_FALSE, &(_camera->getProjMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uViewMat"), 1, GL_FALSE, &(_camera->getViewMat()[0][0]));

    glm::mat4 sliceModel;
    //sliceModel = glm::translate(sliceModel, glm::vec3(0.0f, 0.0f, 0.0f));
    //sliceModel = glm::rotate(sliceModel, (float)glfwGetTime() * 30, glm::vec3(0.0f, 1.0f, 0.0f));
    sliceModel = glm::scale(_modelMat, glm::vec3(1.0f, -1.0f, 0.5f));
    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uModelMat"), 1, GL_FALSE, &sliceModel[0][0]);

    glUniform1i(glGetUniformLocation(program_ray, "uSampler_tex"), 0);
    glUniform1i(glGetUniformLocation(program_ray, "uSampler_trans"), 1);
    glUniform1f(glGetUniformLocation(program_ray, "sample_step_inverse"), adjustParam[0]);
    glUniform1f(glGetUniformLocation(program_ray, "val_threshold"),adjustParam[1]);
    glUniform1f(glGetUniformLocation(program_ray, "brightness"), adjustParam[2]);

    float lightIa[3] = { 0.3,0.3,0.3 };
    float lightId[3] = { 0.7,0.7,0.7 };
    float lightIs[3] = { 0.2,0.2,0.2 };

    glUniform3fv(glGetUniformLocation(program_ray, "Light.Ia"), 1, lightIa);
    glUniform3fv(glGetUniformLocation(program_ray, "Light.Id"), 1, lightId);
    glUniform3fv(glGetUniformLocation(program_ray, "Light.Is"), 1, lightIs);
    glUniform1f(glGetUniformLocation(program_ray, "shiness"), 32.0f);

    glUniform1i(glGetUniformLocation(program_ray, "u_use_color_transfer"), use_color_tranfer);
    glUniform1i(glGetUniformLocation(program_ray, "u_use_ligting"), use_lighting);

    //clipping plane
    glUniform3fv(glGetUniformLocation(program_ray, "u_clip_plane"), 6, cplane_points_);
//    glUniform1i(glGetUniformLocation(program_ray, "u_cpoints_num"), cplane_points_num_);

    glUniform3fv(glGetUniformLocation(program_ray,"step_size"), 3, stepsize_.data.data);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices_num_, GL_UNSIGNED_INT, 0);

//    draw_intersect_plane();
}
void dcmVolumeRender::draw_intersect_plane(){

    glUseProgram(program_plane);
    glUniformMatrix4fv(glGetUniformLocation(program_plane, "uProjMat"), 1, GL_FALSE, &(_camera->getProjMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(program_plane, "uViewMat"), 1, GL_FALSE, &(_camera->getViewMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(program_plane, "uModelMat"), 1, GL_FALSE, &_modelMat[0][0]);

    glBindVertexArray(VAO_PLANE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}