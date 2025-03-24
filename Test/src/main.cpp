#include "ravenpch.h"
#include "Utils/RavenRandom.h"
#include "Core/RavenArray.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Math/glm/glm.hpp"
#include "Math/glm/ext.hpp"
#include "Math/glm/gtx/norm.hpp"

#include "Core/Ray.h"

using std::cout;
using std::cerr;
using std::endl;

using raven::Ray;

int main() {

    std::string str = "raven";
    std::vector<int> vec(5,12);
    
    cout << "############################################## Random Test ############################################## " << endl;
    float new_rand = raven::utils::RavenRandom()();
    cout << "New rand : " << new_rand << endl;
	cout << "############################################## Random Test ############################################## " << endl;
    cout << "\n";
	cout << "############################################## Array Test ############################################## " << endl;

    raven::RavenArray<int> arr;

    for (int i = 0; i < 100; i++)
    {
        arr.append(i);
    }

    arr.truncate();

    cout << "############################################## Array Test ############################################## " << endl;
	cout << "\n";


	cout << "############################################## Vector Test ############################################## " << endl;
    vec3 vec1(10.f,20.f,30.f);
    vec3 vec2(2.f,3.f,4.f);

 //   cout << glm::to_string(vec1) << endl;
 //   
 //   vec3 vec3 = (vec1 + vec2);
 //   cout << "Summation " << vec3 << endl;
 //   
 //   vec3 = vec1 - vec2;
 //   cout << "Difference " << vec3 << endl;
 //   
 //   vec3 = vec1 * vec2;
 //   cout << "Multiply " << vec3 << endl;
 //   
 //   vec3 = vec1 / vec2;
 //   cout << "Division " << vec3 << endl;

 //   vec1 += vec2 += vec2; // Operation Chaining

	//cout << "Operation Chaining" << vec1 << endl;

	cout << "############################################## Ray Test ############################################## " << endl;

    Ray ray(vec1, vec2);
    Ray ray2;

    cout << ray << endl;

	cout << "############################################## Ray Test ############################################## " << endl;


    return 0;


	//HitRecord rec;
	//bool is_a_hit = false;
	//float t_max;

	//vec3 dir(0.f, 0.f, -1.f);

	//raven::RavenArray<Shape*> shapes;

	//shapes.append(new Sphere(vec3(250.f, 250.f, -100.f), 150.f, rgb(0.2f, 0.2f, 0.8f)));

	//shapes.append(new Triangle(vec3(300.f, 400.f, -800.f), vec3(10.0f, 100.f, -1000.f), vec3(450.f, 20.f, -1000.f), rgb(.8f, .2f, .2f)));

	//raven::Image image(500, 500);

	//for (int i = 0; i < 500; i++)
	//{
	//	for (int j = 0; j < 500; j++)
	//	{
	//		t_max = 10000.f;
	//		is_a_hit = false;

	//		raven::Ray r(vec3(float(i), float(j), 0), dir);

	//		for (int k = 0; k < shapes.size(); k++)
	//		{
	//			if (shapes[k]->hit(r, 0.0001f, t_max, rec))
	//			{
	//				t_max = rec.m_t;
	//				is_a_hit = true;
	//			}

	//			if (rec.shape == shapes[0])
	//			{
	//				image.set(i, j, rgb(.2f, 0.2f, 0.8f));
	//			}
	//			if (rec.shape == shapes[1])
	//			{
	//				image.set(i, j, rgb(.1f, .2f, .2f));
	//			}
	//		}
	//	}
	//}

	//std::ofstream out;
	//out.open("outputs/test.ppm");
	//image.write_ppm(out);
	//out.close();
	//return 0;
}