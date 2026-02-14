#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ==========================================
// [تعديل رقم 1]: تحديث Vertex Shader
// في الكود الأصلي: كنا نمرر الموقع فقط.
// التعديل: استقبلنا "اللون" (location 1) لنمرره للشيدر التالي.
// ==========================================
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n" // <--- [إضافة] استقبال اللون
"out vec3 ourColor;\n" // <--- [إضافة] مخرج للون
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n" // <--- [إضافة] تمرير اللون
"}\0";

// ==========================================
// [تعديل رقم 2]: تحديث Fragment Shader
// في الكود الأصلي: كان اللون ثابتاً (برتقالي).
// التعديل: استقبال اللون من الـ Vertex + متغير للشفافية (Alpha).
// ==========================================
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n" // <--- [إضافة] استقبال اللون
"uniform float uAlpha;\n" // <--- [إضافة] متغير للتحكم بالشفافية من الـ CPU
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, uAlpha);\n" // استخدام Alpha في اللون النهائي
"}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ==========================================
    // [تعديل رقم 3]: التحكم في Wireframe
    // المكان: داخل دالة المدخلات.
    // الهدف: التبديل بين رسم الخطوط ورسم التعبئة.
    // ==========================================
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // وضع الخطوط (شبكة)
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // وضع التعبئة (الافتراضي)
}

int main()
{
    // --- تهيئة GLFW (نفس الكود الأصلي) ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 4: Advanced Control", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- تهيئة GLEW (نفس الكود الأصلي) ---
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // ==========================================
    // [تعديل رقم 4]: تفعيل قدرات OpenGL المتقدمة
    // المكان: يجب أن تضاف هنا (بعد glewInit وقبل حلقة الرسم).
    // ==========================================
    glEnable(GL_DEPTH_TEST); // تفعيل اختبار العمق (لمنع تداخل الرسم)
    glEnable(GL_BLEND);      // تفعيل الدمج (للشفافية)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // معادلة الدمج

    // --- بناء وتجميع الشيدر (نفس الخطوات الأصلية) ---
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader); // (تم اختصار فحص الأخطاء للتركيز)

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ==========================================
    // [تعديل رقم 5]: تحديث البيانات (المثلثات)
    // التغيير: بدلاً من 3 رؤوس، أصبح لدينا 6 رؤوس (مثلثين).
    // الإضافة: أضفنا 3 قيم للألوان (RGB) بجانب كل موقع (XYZ).
    // التنسيق الجديد للسطر: X, Y, Z, R, G, B
    // ==========================================
    float vertices[] = {
        // المثلث الأول (أحمر - قريب Z=0.0)
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,

         // المثلث الثاني (أزرق - بعيد Z=0.5)
         -0.2f, -0.2f, 0.5f,  0.0f, 0.0f, 1.0f,
          0.8f, -0.2f, 0.5f,  0.0f, 0.0f, 1.0f,
          0.3f,  0.8f, 0.5f,  0.0f, 0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ==========================================
    // [تعديل رقم 6]: تحديث قراءة البيانات (Stride & Pointers)
    // السبب: تغير حجم البيانات في المصفوفة أعلاه.
    // ==========================================

    // السمة 0: الموقع (3 floats) | الخطوة (Stride) أصبحت 6 * float
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // السمة 1: اللون (3 floats) | تبدأ بعد أول 3 أرقام (Offset)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- حلقة الرسم ---
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // ==========================================
        // [تعديل رقم 7]: تنظيف الـ Buffer
        // الإضافة: يجب مسح GL_DEPTH_BUFFER_BIT في كل فريم.
        // ==========================================
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // ==========================================
        // [تعديل رقم 8]: الرسم المنفصل والتحكم بالمتغيرات
        // بدلاً من رسمة واحدة (glDrawArrays)، نرسم على مرحلتين لتغيير الشفافية.
        // ==========================================

        // الحصول على موقع متغير الشفافية
        int alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");

        // أ. رسم المثلث الأزرق (البعيد) أولاً
        glUniform1f(alphaLoc, 1.0f); // قيمة ألفا = 1 (غير شفاف)
        glDrawArrays(GL_TRIANGLES, 3, 3); // ابدأ من الرأس 3، ارسم 3 رؤوس

        // ب. رسم المثلث الأحمر (القريب) ثانياً
        glUniform1f(alphaLoc, 0.6f); // قيمة ألفا = 0.6 (شبه شفاف)
        glDrawArrays(GL_TRIANGLES, 0, 3); // ابدأ من الرأس 0، ارسم 3 رؤوس

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}