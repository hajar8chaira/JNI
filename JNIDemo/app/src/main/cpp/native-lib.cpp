#include <jni.h>
#include <string>
#include <algorithm>
#include <climits>
#include <android/log.h>
#include <vector>

#define LOG_TAG "JNI_DEMO"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// --- Fonctions Natives (Noms Internes pour Extension D) ---

extern "C" jstring native_helloFromJNI(JNIEnv* env, jobject thiz) {
    LOGI("Appel de helloFromJNI en mode dynamique");
    return env->NewStringUTF("Hello from C++ via JNI !");
}

extern "C" jint native_factorial(JNIEnv* env, jobject thiz, jint n) {
    if (n < 0) return -1;
    long long fact = 1;
    for (int i = 1; i <= n; i++) {
        fact *= i;
        if (fact > INT_MAX) return -2;
    }
    return static_cast<jint>(fact);
}

extern "C" jstring native_reverseString(JNIEnv* env, jobject thiz, jstring javaString) {
    if (javaString == nullptr) return env->NewStringUTF("Erreur : chaine nulle");
    const char* chars = env->GetStringUTFChars(javaString, nullptr);
    if (chars == nullptr) return env->NewStringUTF("Erreur JNI");
    std::string s(chars);
    env->ReleaseStringUTFChars(javaString, chars);
    std::reverse(s.begin(), s.end());
    return env->NewStringUTF(s.c_str());
}

extern "C" jint native_sumArray(JNIEnv* env, jobject thiz, jintArray array) {
    if (array == nullptr) return -1;
    jsize len = env->GetArrayLength(array);
    jint* elements = env->GetIntArrayElements(array, nullptr);
    if (elements == nullptr) return -2;
    long long sum = 0;
    for (jsize i = 0; i < len; i++) sum += elements[i];
    env->ReleaseIntArrayElements(array, elements, JNI_ABORT);
    return (sum > INT_MAX) ? -3 : static_cast<jint>(sum);
}

extern "C" jlong native_benchmarkNative(JNIEnv* env, jobject thiz, jintArray array) {
    if (array == nullptr) return 0;
    jsize len = env->GetArrayLength(array);
    jint* elements = env->GetIntArrayElements(array, nullptr);
    if (elements == nullptr) return 0;
    long long sum = 0;
    for (jsize i = 0; i < len; i++) sum += elements[i];
    env->ReleaseIntArrayElements(array, elements, JNI_ABORT);
    return static_cast<jlong>(sum);
}

// Extension A : Multiplication de matrices 2x2 simples
extern "C" jobjectArray native_multiplyMatrices(JNIEnv* env, jobject thiz, jobjectArray m1, jobjectArray m2) {
    jsize size = env->GetArrayLength(m1);
    jclass intArrCls = env->FindClass("[I");
    jobjectArray result = env->NewObjectArray(size, intArrCls, nullptr);

    std::vector<std::vector<int>> res(size, std::vector<int>(size, 0));

    for (int i = 0; i < size; i++) {
        jintArray rowA = (jintArray)env->GetObjectArrayElement(m1, i);
        jint* elementsA = env->GetIntArrayElements(rowA, nullptr);
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                jintArray rowB = (jintArray)env->GetObjectArrayElement(m2, k);
                jint* elementsB = env->GetIntArrayElements(rowB, nullptr);
                res[i][j] += elementsA[k] * elementsB[j];
                env->ReleaseIntArrayElements(rowB, elementsB, JNI_ABORT);
            }
        }
        env->ReleaseIntArrayElements(rowA, elementsA, JNI_ABORT);
    }

    for (int i = 0; i < size; i++) {
        jintArray row = env->NewIntArray(size);
        env->SetIntArrayRegion(row, 0, size, res[i].data());
        env->SetObjectArrayElement(result, i, row);
    }
    return result;
}

// Extension B : Verification de caracteres interdits
extern "C" jboolean native_checkString(JNIEnv* env, jobject thiz, jstring jInput, jstring jForbidden) {
    const char* input = env->GetStringUTFChars(jInput, nullptr);
    const char* forbidden = env->GetStringUTFChars(jForbidden, nullptr);
    bool found = false;
    for (int i = 0; input[i] != '\0' && !found; i++) {
        for (int j = 0; forbidden[j] != '\0'; j++) {
            if (input[i] == forbidden[j]) {
                found = true;
                break;
            }
        }
    }
    env->ReleaseStringUTFChars(jInput, input);
    env->ReleaseStringUTFChars(jForbidden, forbidden);
    return static_cast<jboolean>(found);
}

// --- Enregistrement Dynamique (Extension D) ---

static JNINativeMethod gMethods[] = {
    {"helloFromJNI", "()Ljava/lang/String;", (void*)native_helloFromJNI},
    {"factorial", "(I)I", (void*)native_factorial},
    {"reverseString", "(Ljava/lang/String;)Ljava/lang/String;", (void*)native_reverseString},
    {"sumArray", "([I)I", (void*)native_sumArray},
    {"benchmarkNative", "([I)J", (void*)native_benchmarkNative},
    {"multiplyMatrices", "([[I[[I)[[I", (void*)native_multiplyMatrices},
    {"checkString", "(Ljava/lang/String;Ljava/lang/String;)Z", (void*)native_checkString}
};

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) return JNI_ERR;

    jclass clazz = env->FindClass("com/example/jnidemo/MainActivity");
    if (clazz == nullptr) return JNI_ERR;

    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) return JNI_ERR;

    return JNI_VERSION_1_6;
}

