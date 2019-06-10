#include "dcmRenderer.h"
#include "Color.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <set>
#include <algorithm>
using namespace glm;

bool PointInsideBoundingBox(vec3 p, vec3 aabb_min, vec3 aabb_max){
    if(p.x < aabb_min.x || p.x > aabb_max.x
       ||p.y < aabb_min.y || p.y > aabb_max.y
       ||p.z < aabb_min.z || p.z > aabb_max.z) return false;
    return true;
}

//ray intersect a plane
bool getRayIntersectionPlanePoint(vec3 ray_start, vec3 ray_dir, vec3 plane_point, vec3 plane_norm, vec3& out_point){
    float ray_plane_angle  = dot(ray_dir, plane_norm);
    if(ray_plane_angle == .0f)
        return false;
    //AX + BY + CZ + D = 0
    //D = -(PLANE_POINT * PLANE_NORM)
    //N = RAY_DIR * PLANE_NORM
    out_point =  ray_start - (( dot(plane_norm, ray_start) - dot(plane_point, plane_norm)) / ray_plane_angle) * ray_dir;
    return true;
}
bool getNearestRayAABBIntersection(vec3 ray_start, vec3 ray_dir, vec3 aabbMin, vec3 aabbMax, vec3& nearest_p){
//    float tmin = FLT_MIN, tmax = FLT_MAX;
//    for(int a = 0; a<3; a++){
//        float invD = 1.0f / ray_dir[a];
//        float t0 = (aabbMin[a] - ray_start[a]) * invD;
//        float t1 = (aabbMax[a] - ray_start[a]) * invD;
//        if(invD < .0f)  std::swap(t0, t1);
//        tmin = max(t0, tmin); tmax = min(t1, tmax);
//        if(tmax <= tmin) return false;
//    }
    vec3 pop = aabbMin, interset_p;
    bool is_intersect = false;
    int best_pid;
    float c_dist, best_dist = FLT_MAX;
    const vec3 face_norm[6] = {vec3(1,0,0), vec3(0,0,1),vec3(0,1,0),
                               vec3(-1,0,0), vec3(0,0,-1), vec3(0,-1,0)};
    for(int i=0; i<6; i++) {
        if (i == 3) pop = aabbMax;
        if (getRayIntersectionPlanePoint(ray_start, ray_dir, pop, face_norm[i], interset_p)
            && PointInsideBoundingBox(interset_p, aabbMin, aabbMax)){
            c_dist = glm::distance(ray_start, interset_p);
            if (c_dist < best_dist) {
                c_dist = best_dist;
                best_pid = i;
                nearest_p = interset_p;
                is_intersect = true;
            }
        }
    }
    return is_intersect;
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
    if(getRayIntersectionPlanePoint(aabb_min, dir, p, p_norm, p_rp)
    && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BACK].push_back(pid);
        face_map[BOTTOM].push_back(pid);
        pid++;
    }

    if(getRayIntersectionPlanePoint(vec3(aabb_min.x, aabb_max.y, aabb_min.z), dir, p, p_norm, p_rp)
    && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BACK].push_back(pid);
        face_map[UP].push_back(pid);
        pid++;
    }
    if(getRayIntersectionPlanePoint(vec3(aabb_min.x, aabb_min.y, aabb_max.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[FRONT].push_back(pid);
        face_map[BOTTOM].push_back(pid);
        pid++;
    }

    if(getRayIntersectionPlanePoint(vec3(aabb_min.x, aabb_max.y, aabb_max.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[FRONT].push_back(pid);
        face_map[UP].push_back(pid);
        pid++;
}

    // Test edges along Y axis, pointing up.
    dir = vec3(0.f, aabb_max.y - aabb_min.y, 0.f);
    if(getRayIntersectionPlanePoint(vec3(aabb_min.x, aabb_min.y, aabb_min.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BACK].push_back(pid);
        face_map[LEFT].push_back(pid);
        pid++;
    }
    if(getRayIntersectionPlanePoint(vec3(aabb_max.x, aabb_min.y, aabb_min.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BACK].push_back(pid);
        face_map[RIGHT].push_back(pid);
        pid++;
    }
    if(getRayIntersectionPlanePoint(vec3(aabb_min.x, aabb_min.y, aabb_max.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[FRONT].push_back(pid);
        face_map[LEFT].push_back(pid);
        pid++;
    }
    if(getRayIntersectionPlanePoint(vec3(aabb_max.x, aabb_min.y, aabb_max.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[FRONT].push_back(pid);
        face_map[RIGHT].push_back(pid);
        pid++;
    }

    // Test edges along Z axis, pointing forward.
    dir = vec3(0.f, 0.f, aabb_max.z - aabb_min.z);
    if(getRayIntersectionPlanePoint(vec3(aabb_min.x, aabb_min.y, aabb_min.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[LEFT].push_back(pid);
        face_map[BOTTOM].push_back(pid);
        pid++;
    }
    if(getRayIntersectionPlanePoint(vec3(aabb_max.x, aabb_min.y, aabb_min.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[BOTTOM].push_back(pid);
        face_map[RIGHT].push_back(pid);
        pid++;
    }
    if(getRayIntersectionPlanePoint(vec3(aabb_min.x, aabb_max.y, aabb_min.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
        polygon.push_back(std::make_pair(p_rp, pid));
        face_map[UP].push_back(pid);
        face_map[LEFT].push_back(pid);
        pid++;
    }
    if(getRayIntersectionPlanePoint(vec3(aabb_max.x, aabb_max.y, aabb_min.z), dir, p, p_norm, p_rp)
       && PointInsideBoundingBox(p_rp, aabb_min, aabb_max)){
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
        return dot(cross_v, p_norm)<0;
    });
}


dcmVolumeRender::dcmVolumeRender(AAssetManager *assetManager):
        cubeRenderer(assetManager){
    new assetLoader(assetManager);
    _modelMat = glm::scale(_modelMat, scale_origin);
}

void dcmVolumeRender::addImage(GLubyte * img, float location) {
    images_.push_back(new dcmImage(
            img,
            location));
}
void colorConverter(int hexValue,float* color){
    color[0] = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
    color[1] = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
    color[2] = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte
    color[3] = 1.0f;
}
void dcmVolumeRender::setting_1D_texture(){
    glActiveTexture(GL_TEXTURE1);
    //create texture object
    glGenTextures(1, &trans_texid);
    glBindTexture(GL_TEXTURE_2D, trans_texid);
    //bind current texture object and set the data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    float* m_transfer_color = new float[4 * Color_num_transfer];

    for(int i=0; i<Color_num_transfer; i++)
        colorConverter(hexcolor_transfer[i], &m_transfer_color[4*i]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Color_num_transfer, 1, 0, GL_RGBA, GL_FLOAT, m_transfer_color);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    delete []m_transfer_color;
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
    glClearColor(0.5,0.5,0.5,1.0);
}
void dcmVolumeRender::onViewCreated(){
    program_ray = assetLoader::instance()->createGLShaderProgramFromFile("shaders/raycastVolume.vert", "shaders/raycastVolume.frag");
    program_plane = assetLoader::instance()->createGLShaderProgramFromFile("shaders/IntersectionPlane.vert", "shaders/IntersectionPlane.frag");

    program_texture = assetLoader::instance()->createGLShaderProgramFromFile("shaders/textureVolume.vert", "shaders/textureVolume.frag");
    if(!program_ray || !program_texture || !program_plane)
        LOGE("===Failed to create shader program===");
}

void dcmVolumeRender::updateTexCoords(GLfloat* vertices, glm::vec3 p){
    vertices[0] = p.x; vertices[1] = p.y; vertices[2] = p.z;
    vertices[3] = floorf((p.x + 0.5f) *volume_size.x) * stepsize_.x;
    vertices[4] = floorf((p.y + 0.5f) *volume_size.y) * stepsize_.y;
    vertices[5] = floorf((p.z + 0.5f) *volume_size.z) * stepsize_.z;
//    vertices[3] = p.x + 0.5f;vertices[4] = p.y + 0.5f;vertices[5] = p.z + 0.5f;
}
//update vertices, indices, and corresponding numbers
void dcmVolumeRender::updateGeometry(std::vector<Polygon> polygon, PolygonMap polygon_map, std::vector<int> rpoints){
//    delete(vertices_);
//    delete(indices_);
    //create new vertices
    int rp_num = rpoints.size();
    vertices_num_ = rp_num + polygon.size();
    std::vector<int>id_map(vertices_num_, 0);
//    int id_map[vertices_num_] = {0};

    GLfloat * c_vertices_ = new GLfloat[VAO_DATA_LEN*vertices_num_];
    int nidx = 0;
    for(auto id:rpoints){
//        c_vertices_[VAO_DATA_LEN*nidx] = sVertex[id*VAO_DATA_LEN];c_vertices_[VAO_DATA_LEN*nidx+1] = sVertex[id*3+1];c_vertices_[3*nidx+2] = sVertex[id*3+2];
        memcpy(c_vertices_+VAO_DATA_LEN*nidx, sVertex+VAO_DATA_LEN*id, VAO_DATA_LEN*sizeof(GL_FLOAT));
        id_map[nidx] = id; nidx++;
    }
    for(auto point:polygon){
//        c_vertices_[VAO_DATA_LEN*nidx] = point.first.x;c_vertices_[VAO_DATA_LEN*nidx+1] = point.first.y;c_vertices_[VAO_DATA_LEN*nidx+2] = point.first.z;
        updateTexCoords(c_vertices_+VAO_DATA_LEN*nidx, point.first);
        id_map[nidx] = point.second; nidx++;
    }
    memcpy(vertices_, c_vertices_, VAO_DATA_LEN * vertices_num_*sizeof(GLfloat));

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
                face_points.push_back(std::make_pair(vec3(vertices_[VAO_DATA_LEN*cid], vertices_[VAO_DATA_LEN*cid+1], vertices_[VAO_DATA_LEN*cid+2]), cid));
            }
            for(auto id:uset){
                int cid = std::distance(id_map.begin(), std::find(id_map.begin(), id_map.begin()+rp_num, id));
                face_points.push_back(std::make_pair(vec3(vertices_[VAO_DATA_LEN*cid], vertices_[VAO_DATA_LEN*cid+1], vertices_[VAO_DATA_LEN*cid+2]), cid));
            }

            vec3 origin = face_points[0].first;
            vec3 p_norm = getPlaneNormal(face_points[0].first, face_points[1].first, face_points[2].first);
            std::sort(face_points.begin(), face_points.end(), [&](const Polygon& p1, const Polygon& p2)->bool{
                vec3 cross_v = glm::cross(p1.first - origin, p2.first - origin);
                return dot(cross_v, p_norm)<0;
            });


            //face points are sorted counter_clock_wise
            for(int i=0; i<face_points.size()-2; i++){
                GLuint indice[] = {(GLuint)face_points[0].second, (GLuint)face_points[i+1].second, (GLuint)face_points[i+2].second};
                //check if two points are the same
//                vec3 p0 = face_points[0].first,
//                     p1 = face_points[i+1].first,
//                     p2 = face_points[i+2].first;
//                if(glm::all(glm::equal(p0, p1)) || glm::all(glm::equal(p0, p2)) ||glm::all(glm::equal(p1, p2))) continue;
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

//    indices_ = memcpy(c_indices.begin(), );
    indices_num_ = c_indices.size();
    memcpy(indices_, c_indices.data(), indices_num_ * sizeof(GLuint));
    delete []c_vertices_;
}
void dcmVolumeRender::updateVBOData(){
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, VAO_DATA_LEN * vertices_num_  *sizeof(GL_FLOAT), vertices_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_num_  *sizeof(GL_UNSIGNED_INT), indices_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void dcmVolumeRender::restore_original_cube(){
    indices_num_ = 36;
    vertices_num_ = 8;
    memcpy(vertices_, sVertex, sizeof(GLfloat) * VAO_DATA_LEN * vertices_num_);
    memcpy(indices_, sIndices, sizeof(GLuint) * indices_num_);
}
void dcmVolumeRender::setCuttingPlane_texturebased(float percent){
    slice_start_idx = (int)m_VAOs.size() * percent;
}
void dcmVolumeRender::setCuttingPlane(float percent){
//        float points_[9] ={0,0.5,0.5,
//                        0.5,0.5,0,
//                        0.5,0,0.5};
//
//    std::vector<vec3> plane_points;
//    polygon.clear();
//    polygon_map.clear();
//    for(int i=0; i<3; i++)
//        plane_points.push_back(vec3(points_[3*i], points_[3*i+1],points_[3*i+2]));
//    vec3 p_norm = getPlaneNormal(plane_points[0], plane_points[1], plane_points[2]);
    is_in_deeper = percent>0.5f;
    polygon.clear();
    polygon_map.clear();

    //get the nearest - farest vertex
    //s1. trans eye position back to model-coord
    glm::vec3 eye_model3 = _camera->getCameraPosition(_modelMat);
    vec3 vdir_model = -glm::normalize(eye_model3);

    if(!glm::any(glm::equal(_camera->getViewDirection(), last_cutting_norm))){

        //update cutting nearest - fartest position
        vec3 nearest_plane_intersection;
        if(getNearestRayAABBIntersection(eye_model3, vdir_model,
                                         vec3(-0.5), vec3(0.5),
                                         nearest_plane_intersection)){
            float hdist = glm::distance(_camera->getViewCenter() , nearest_plane_intersection);
            hdist*=1.3f;
            start_cutting = -hdist * vdir_model;
            cutting_length = 2.0f * hdist;
            last_cutting_norm = _camera->getCameraPosition();
            is_cutting = true;
        } else{
            is_cutting = false;
            LOGE("=====is not cutting, eye pos: %f, %f, %f, dir %f %f %f", eye_model3.x, eye_model3.y, eye_model3.z, vdir_model.x, vdir_model.y, vdir_model.z);

        }
    }
    if(!is_cutting) return;
    vec3 pop_model = start_cutting + percent * cutting_length * vdir_model;
    updateCuttingPlane(pop_model, vdir_model);
}

//p and p norm should be in model space
void dcmVolumeRender::updateCuttingPlane(glm::vec3 p, glm::vec3 p_norm){
    //view_dir and p_norm should be on the same side
    vec3 view_model_dir = glm::inverse(mat3(_modelMat)) * _camera->getViewDirection();
    if(dot(p_norm, view_model_dir) < 0)
        p_norm = -p_norm;

    getIntersectionPolygon(p, p_norm, vec3(-0.5f), vec3(0.5f), polygon, polygon_map);
    if(polygon.empty()){
        vec3 eye_model = _camera->getCameraPosition(_modelMat);
        if(is_in_deeper){
            vertices_num_ = 0; indices_num_ =0;
        }else{
            restore_original_cube();
        }
        updateVBOData(); return;
    }
    //Test 8 points which should be remove
    std::vector<int> rpoints;
    std::vector<vec3> rpoints_values;

    for(int i=0;i<8;i++){
        vec3 vertex = vec3(sVertex[VAO_DATA_LEN*i], sVertex[VAO_DATA_LEN*i+1],sVertex[VAO_DATA_LEN*i+2]);//point to test
        if(dot(vertex - p, p_norm) >= 0) {
            rpoints.push_back(i);
            rpoints_values.push_back(vec3(sVertex[VAO_DATA_LEN * i], sVertex[VAO_DATA_LEN * i+1],sVertex[VAO_DATA_LEN * i+2]));
        }

    }
    std::deque<int> polygon_to_be_erased;
    //check if new points overlap with original vertices
    for(int idx = polygon.size();idx >-1; idx--){
        auto got = std::find(rpoints_values.begin(), rpoints_values.end(), polygon[idx].first);
        if(got != rpoints_values.end()){
            polygon_to_be_erased.push_front(idx);
            for(auto face= polygon_map.begin(); face!=polygon_map.end(); face++){
                auto tmpVec = face->second;
                auto gotid = std::find(tmpVec.begin(), tmpVec.end(), idx);
                if(gotid!=tmpVec.end()){
                    int cid = std::distance(tmpVec.begin(), gotid);
                    tmpVec.erase(tmpVec.begin() + cid);
                    face->second = tmpVec;
                }
            }
        }
    }

//    for(auto idx:polygon_to_be_erased){
////        polygon.erase(polygon.begin() + idx);
//        for(auto face= polygon_map.begin(); face!=polygon_map.end(); face++){
//            auto tmpVec = face->second;
//            auto gotid = std::find(tmpVec.begin(), tmpVec.end(), idx);
//            if(gotid!=tmpVec.end()){
//                int cid = std::distance(tmpVec.begin(), gotid);
//                tmpVec.erase(tmpVec.begin() + cid);
//                face->second = tmpVec;
//            }
//        }
//    }

    updateGeometry(polygon, polygon_map, rpoints);
    updateVBOData();
}
void dcmVolumeRender::initGeometry() {
    stepsize_ = vec3(1.0f / img_width, 1.0f / img_height, 1.0f/dimensions);
    volume_size = vec3(img_width, img_height, dimensions);
    vertices_ = new GLfloat[MAX_VERTEX_NUM * VAO_DATA_LEN];
    indices_ = new GLuint[MAX_INDICE_NUM];

    restore_original_cube();
////////////////////////////////////////////////////
//    for(int i=0; i<polygon.size();i++)
//        LOGE("====INTERSECTION id : %d, pos:  %f, %f, %f\n", polygon[i].second, polygon[i].first.x, polygon[i].first.y,polygon[i].first.z);
//
//    for(auto item : polygon_map){
//        LOGE("====== face %d", item.first);
//        for(auto id : item.second){
//            LOGE("===id: %d", id);
//        }
//        LOGE("====\n");
//    }


//    for(int i=0; i<rpoints.size(); i++) LOGE("====keep point: %d", rpoints[i]);

    // rpoint are in accending order
    //for each face, create new vertices and indices

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
    //Generate the VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, VAO_DATA_LEN*MAX_VERTEX_NUM* sizeof(GL_FLOAT), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VAO_DATA_LEN * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VAO_DATA_LEN * sizeof(float), (void*)(3 * sizeof(float)));

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICE_NUM * sizeof(GL_UNSIGNED_INT), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    setCuttingPlane();
}

void init_static_buffers(GLuint &vao, float* vertices, int vertex_num, unsigned int* indices, int indice_num){
    glGenVertexArrays(1, &vao);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray((GLuint)vao);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * vertex_num, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indice_num, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
void dcmVolumeRender::initGeometry_texturebased() {
    size_t dims = NEED_SLCIES;
    m_VAOs = std::vector<GLuint >(dims);

    float dimension_inv = 1.0f / dims;

    const float mVertices[] = {
            // positions		// texture coords
            -0.5f,  -0.5f, .0,	0.0f, 0.0f, .0,   // top right
            0.5f, -0.5f, .0,	1.0f, 0.0f, .0,  // bottom right
            0.5f, 0.5f, .0,		1.0f, 1.0f, .0,  // bottom left
            -0.5f,  0.5f, .0,	0.0f, 1.0f,  .0  // top left
    };
    unsigned int indices[] = {
            0,3,2,
            2,1,0
    };
    float vertices[24];
    float mappedZVal = -scale_inv, zTex = .0f;
    for (int i = 0; i < dims; i++){
        memcpy(vertices, mVertices, sizeof(float)*24);
        for(int j=0;j<4;j++){vertices[6*j + 2] = mappedZVal;vertices[6*j + 5] = zTex; }
        mappedZVal+=2.0 * dimension_inv* scale_inv; zTex+=dimension_inv;

        init_static_buffers(m_VAOs[i], vertices, 4, indices, 6);
    }

    //close gap
    float close_points[] = {//World					//Color
            -0.5f,-0.5f,scale_inv, 0.0f,0.0f,1.0f,		//x0, y0, z1, //	//v0
            0.5f,-0.5f,scale_inv, 1.0f,0.0f,1.0f,		//x1,y0,z1, //	//v1
            0.5f,0.5f,scale_inv,	1.0f,1.0f,1.0f,		//x1, y1, z1,//	//v2
            -0.5f,0.5f,scale_inv,0.0f,1.0f,1.0f,		//x0,y1,z1, //	//v3
            -0.5f,-0.5f,-scale_inv,0.0f,0.0f,0.0f,	//x0,y0,z0,//	//v4
            0.5f,-0.5f,-scale_inv,1.0f,0.0f,0.0f,		//x1,y0,z0,//	//v5
            0.5f,0.5f,-scale_inv,1.0f,1.0f,0.0f,	//x1,y1,z0, //	//v6
            -0.5f,0.5f,-scale_inv,0.0f,1.0f,0.0f,		//x0,y1,z0//	//v7
    };
    unsigned int Sclose_indices[] = {
            4,0,3,4,3,7,	//left
            1,5,6,1,6,2,	//right
            3,2,6,3,6,7,	//top
            4,5,1,4,1,0,	//bottom
    };

    init_static_buffers(close_VAO, close_points, 8, Sclose_indices, 24);
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
            onTexturebasedDraw_dense();
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
void dcmVolumeRender::onTexturebasedDraw_dense(){
    updateVBOData();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    glBindVertexArray(VAO);
    glDrawElements(RenderMode[gl_draw_mode_id], indices_num_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void dcmVolumeRender::onTexturebasedDraw(){

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    glEnable(GL_DEPTH_TEST);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    glUniform1i(glGetUniformLocation(program_texture, "uSampler_trans"), 1);
    glUniform3f(glGetUniformLocation(program_texture, "uVolumeSize"), volume_size.x, volume_size.y, volume_size.z);

    glUniform1i(glGetUniformLocation(program_texture, "u_use_color_transfer"), use_color_tranfer);
    glUniform1f(glGetUniformLocation(program_texture, "uOpacityThreshold"), adjustParam[3]);

    if(_camera->getViewDirection().z <0){
//        glUniform1i(glGetUniformLocation(program_texture, "u_is_front"), true);
        glFrontFace(GL_CCW);
        for (int id = 0; id <m_VAOs.size() - slice_start_idx; id++) {
            glBindVertexArray(m_VAOs[id]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }
    else{
        glFrontFace(GL_CW);
        for (int id = m_VAOs.size()-1; id >= slice_start_idx; id--) {
            glBindVertexArray(m_VAOs[id]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }

//    if(slice_start_idx == 0){
//        glBindVertexArray(close_VAO);
//        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
//    }

    glUseProgram(0);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}
void dcmVolumeRender::onRaycastDraw(){
    updateVBOData();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, volume_texid);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, trans_texid);

    glUseProgram(program_ray);

    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uProjMat"), 1, GL_FALSE, &(_camera->getProjMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uViewMat"), 1, GL_FALSE, &(_camera->getViewMat()[0][0]));

    glUniformMatrix4fv(glGetUniformLocation(program_ray, "uModelMat"), 1, GL_FALSE, &_modelMat[0][0]);

    glUniform1i(glGetUniformLocation(program_ray, "uSampler_tex"), 0);
    glUniform1i(glGetUniformLocation(program_ray, "uSampler_trans"), 1);
    glUniform1f(glGetUniformLocation(program_ray, "sample_step_inverse"), adjustParam[0]);
    glUniform1f(glGetUniformLocation(program_ray, "val_threshold"),adjustParam[1]);
    glUniform1f(glGetUniformLocation(program_ray, "brightness"), adjustParam[2]);
    glUniform1f(glGetUniformLocation(program_ray, "OpacityThreshold"), adjustParam[3]);
    float lightIa[3] = { 0.8,0.8,0.8 };
    float lightId[3] = { 0.7,0.7,0.7 };
    float lightIs[3] = { 0.5,0.5,0.5 };

    glUniform3fv(glGetUniformLocation(program_ray, "Light.Ia"), 1, lightIa);
    glUniform3fv(glGetUniformLocation(program_ray, "Light.Id"), 1, lightId);
    glUniform3fv(glGetUniformLocation(program_ray, "Light.Is"), 1, lightIs);
    glUniform1f(glGetUniformLocation(program_ray, "shiness"), 32.0f);

    glUniform1i(glGetUniformLocation(program_ray, "u_use_color_transfer"), use_color_tranfer);
    glUniform1i(glGetUniformLocation(program_ray, "u_use_ligting"), use_lighting);
    glUniform1i(glGetUniformLocation(program_ray, "u_use_interpolation"), use_interpolation);
    glUniform1i(glGetUniformLocation(program_ray, "u_draw_naive"), use_simple_cube);

    //clipping plane
//    glUniform3fv(glGetUniformLocation(program_ray, "u_clip_plane"), 6, cplane_points_);
//    glUniform1i(glGetUniformLocation(program_ray, "u_cpoints_num"), cplane_points_num_);

    glUniform1f(glGetUniformLocation(program_ray, "volumex"), volume_size.x);
    glUniform1f(glGetUniformLocation(program_ray, "volumey"), volume_size.y);
    glUniform1f(glGetUniformLocation(program_ray, "volumez"), volume_size.z);
    glBindVertexArray(VAO);

    glDrawElements(RenderMode[gl_draw_mode_id], indices_num_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
//    LOGE("=====eyeposz: %f", _camera->getCameraPosition().z);
//    draw_intersect_plane();


//    GLubyte *data = (GLubyte*)malloc(4 * 1080 * 1920);
//    if( data ) {
//        glReadPixels(0, 0, 1080, 1920, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    }
//    for(int i=0; i<4*1080 * 1920; i++){
//        if(data[i] != GLubyte(0))
//            LOGE("====value %d", data[i]);
//    }
}
void dcmVolumeRender::draw_intersect_plane(){

    glUseProgram(program_plane);
    glUniformMatrix4fv(glGetUniformLocation(program_plane, "uProjMat"), 1, GL_FALSE, &(_camera->getProjMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(program_plane, "uViewMat"), 1, GL_FALSE, &(_camera->getViewMat()[0][0]));
    glUniformMatrix4fv(glGetUniformLocation(program_plane, "uModelMat"), 1, GL_FALSE, &_modelMat[0][0]);

    glBindVertexArray(VAO_PLANE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}