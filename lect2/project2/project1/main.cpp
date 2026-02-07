#include <iostream>

// 1. تضمين المكتبات
// ملاحظة: يجب تضمين GLEW قبل GLFW دائماً
#define GLEW_STATIC // لأننا نستخدم مكتبة glew32s.lib (Static)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// إعدادات النافذة
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// --- 2. كود المظللات (Shaders) ---

// A. كود Vertex Shader
// وظيفته: تحديد موقع رؤوس المثلث في الفضاء
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n" // نستقبل البيانات في الموقع 0
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // نمرر الموقع كما هو
"}\0";

// B. كود Fragment Shader
// وظيفته: تحديد لون البكسلات (هنا برتقالي)
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" // RGBA: برتقالي
"}\n\0";

// دالة لمعالجة تغيير حجم النافذة من قبل المستخدم
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// دالة لمعالجة المدخلات (مثل الضغط على زر ESC)
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    // --- 3. تهيئة GLFW وإعداد النافذة ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // إصدار OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // النمط الحديث

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // خاص بأجهزة ماك
#endif

    // إنشاء كائن النافذة
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL - First Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // جعل هذه النافذة هي سياق الرسم الحالي
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // ربط دالة تغيير الحجم

    // --- 4. تهيئة GLEW ---
    glewExperimental = GL_TRUE; // تفعيل التقنيات الحديثة
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // --- 5. بناء وتجميع برنامج الشيدر (Shader Program) ---

    // أ. تجميع Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // فحص الأخطاء
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // ب. تجميع Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // فحص الأخطاء
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // ج. ربط الشيدرز في برنامج واحد (Shader Program)
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // فحص أخطاء الربط
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // حذف الشيدرز المنفصلة لأننا ربطناها في البرنامج ولم نعد بحاجة لها
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- 6. تعريف البيانات (المثلث) ---
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // يسار أسفل
         0.5f, -0.5f, 0.0f, // يمين أسفل
         0.0f,  0.5f, 0.0f  // أعلى منتصف
    };

    // --- 7. إعداد VBO و VAO ---
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // توليد المعرفات
    glGenBuffers(1, &VBO);

    // خطوة مهمة جداً: اربط VAO أولاً قبل أي شيء آخر
    glBindVertexArray(VAO);

    // اربط VBO وانسخ البيانات إليه
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // شرح البيانات لـ OpenGL (Linking Vertex Attributes)
    // 0: الموقع في الشيدر
    // 3: حجم البيانات (3 floats لكل نقطة)
    // GL_FLOAT: نوع البيانات
    // GL_FALSE: لا نريد تطبيع البيانات
    // 3 * sizeof(float): الخطوة (Stride)
    // (void*)0: الإزاحة (بداية البيانات)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // تفعيل السمة رقم 0

    // فك الربط (اختياري ولكنه ممارسة جيدة)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- 8. حلقة الرسم (Render Loop) ---
    while (!glfwWindowShouldClose(window))
    {
        // أ. معالجة المدخلات
        processInput(window);

        // ب. التنظيف (لون الخلفية)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // لون تركوازي غامق
        glClear(GL_COLOR_BUFFER_BIT);

        // ج. الرسم
        glUseProgram(shaderProgram); // تفعيل الشيدر
        glBindVertexArray(VAO); // تفعيل كائن المثلث
        glDrawArrays(GL_TRIANGLES, 0, 3); // ارسم مثلثاً من 3 رؤوس
        // glBindVertexArray(0); // لا داعي لفك الربط في كل فريم

        // د. تبديل الـ Buffers ومعالجة الأحداث
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- 9. التنظيف النهائي ---
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}