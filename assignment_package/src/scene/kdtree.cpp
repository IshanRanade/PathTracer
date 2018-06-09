#include "kdtree.h"

KDNode::KDNode()
    : leftChild(nullptr), rightChild(nullptr), axis(0), minCorner(), maxCorner(), particles()
{}

KDNode::~KDNode()
{
    delete leftChild;
    delete rightChild;
}

KDTree::KDTree()
    : root(nullptr)
{}

KDTree::~KDTree()
{
    delete root;
}

// Comparator functions you can use with std::sort to sort vec3s along the cardinal axes
bool xSort(Photon a, Photon b) { return a.pos.x < b.pos.x; }
bool ySort(Photon a, Photon b) { return a.pos.y < b.pos.y; }
bool zSort(Photon a, Photon b) { return a.pos.z < b.pos.z; }


void KDTree::build(const std::vector<Photon> *points)
{
    float smallestXValue = std::numeric_limits<float>::max();
    float smallestYValue = std::numeric_limits<float>::max();
    float smallestZValue = std::numeric_limits<float>::max();
    float largestXValue = std::numeric_limits<float>::min();
    float largestYValue = std::numeric_limits<float>::min();
    float largestZValue = std::numeric_limits<float>::min();

    for(Photon point : *points) {
        if(point.pos.x < smallestXValue) {
            smallestXValue = point.pos.x;
        }
        if(point.pos.x > largestXValue) {
            largestXValue = point.pos.x;
        }
        if(point.pos.y < smallestYValue) {
            smallestYValue = point.pos.y;
        }
        if(point.pos.y > largestYValue) {
            largestYValue = point.pos.y;
        }
        if(point.pos.z < smallestZValue) {
            smallestZValue = point.pos.z;
        }
        if(point.pos.z > largestZValue) {
            largestZValue = point.pos.z;
        }
    }

    if(points->size() > 0) {
        std::vector<Photon> A;
        for(Photon point: *points) {
            A.push_back(point);
        }

        std::sort(A.begin(), A.end(), xSort);

        this->minCorner = glm::vec3(smallestXValue, smallestYValue, smallestZValue);
        this->maxCorner = glm::vec3(largestXValue, largestYValue, largestZValue);
        this->root = buildHelper(A, 0, this->minCorner, this->maxCorner);
    }
}

KDNode* KDTree::buildHelper(std::vector<Photon> points, int depth, glm::vec3 minCorner, glm::vec3 maxCorner) {
    if(points.size() == 1) {
        KDNode* newNode = new KDNode();
        newNode->minCorner = points[0].pos;
        newNode->maxCorner = points[0].pos;
        newNode->axis = depth;
        newNode->particles.push_back(points[0]);
        return newNode;
    }

    std::vector<Photon> A;
    std::vector<Photon> B;
    bool (*comp)(Photon, Photon);

    if(depth == 0){
        comp = &ySort;
    } else if(depth == 1) {
        comp = &zSort;
    } else if(depth == 2) {
        comp = &xSort;
    }

    for(int i = 0; i < points.size(); ++i) {
        if(i < points.size() / 2) {
            A.push_back(points[i]);
        } else {
            B.push_back(points[i]);
        }
    }

    glm::vec3 newMinCornerLeft = minCorner;
    newMinCornerLeft[depth] = (A[0]).pos[depth];
    glm::vec3 newMaxCornerLeft = maxCorner;
    newMaxCornerLeft[depth] = (A[A.size() - 1]).pos[depth];

    glm::vec3 newMinCornerRight = minCorner;
    newMinCornerRight[depth] = (B[0]).pos[depth];
    glm::vec3 newMaxCornerRight = maxCorner;
    newMaxCornerRight[depth] = (B[B.size() - 1]).pos[depth];

    KDNode* newNode = new KDNode();
    newNode->minCorner = minCorner;
    newNode->maxCorner = maxCorner;
    newNode->axis = depth;

    std::sort(A.begin(), A.end(), comp);
    std::sort(B.begin(), B.end(), comp);

    newNode->leftChild  = buildHelper(A, (depth+1) % 3, newMinCornerLeft, newMaxCornerLeft);
    newNode->rightChild = buildHelper(B, (depth+1) % 3, newMinCornerRight, newMaxCornerRight);
    return newNode;
}

std::vector<Photon> KDTree::particlesInSphere(glm::vec3 c, float r)
{
    //TODO
    std::vector<Photon> result;
    searchSubtree(&result, this->root, c, r, c - glm::vec3(r, r, r), c + glm::vec3(r, r, r));
    //reportSubtree(&result, this->root, c, r);
    return result;
}

void KDTree::reportSubtree(std::vector<Photon>* result, KDNode* node, glm::vec3 c, float r) {
    if(node != nullptr) {
        if(node->leftChild == nullptr && node->rightChild == nullptr) {
            if(glm::distance((node->particles[0]).pos, c) < r) {
                result->push_back((node->particles[0]));
            }
        }

        reportSubtree(result, node->leftChild, c, r);
        reportSubtree(result, node->rightChild, c, r);
    }
}

void KDTree::searchSubtree(std::vector<Photon>* result, KDNode* node, glm::vec3 c, float r, glm::vec3 min, glm::vec3 max) {
    if(node != nullptr) {
        if(node->leftChild == nullptr && node->rightChild == nullptr) {
            if(glm::distance((node->particles[0]).pos, c) < r) {
                result->push_back((node->particles[0]));
            }
        } else {
            if(   !((node->minCorner[0] > max[0] || node->maxCorner[0] < min[0]) ||
                    (node->minCorner[1] > max[1] || node->maxCorner[1] < min[1]) ||
                    (node->minCorner[2] > max[2] || node->maxCorner[2] < min[2]))) {
                searchSubtree(result, node->leftChild, c, r, min, max);
                searchSubtree(result, node->rightChild, c, r, min, max);
            }
        }
    }
}

void KDTree::clear()
{
    delete root;
    root = nullptr;
}
