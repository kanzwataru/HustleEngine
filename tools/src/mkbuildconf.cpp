/*


*/
#include <cstdio>
#include <vector>
#include <variant>
#include <string>

using Choices = std::vector<std::string>;
using Value = std::variant<bool, std::string, Choices>;
using Settings = std::vector<std::pair<std::string, Value>>;

Settings build_settings = {
    {"TARGET_PLATFORM", Choices{"unix", "dos"}},
    {"DEBUG_BUILD", false},
};

std::string generate_make_fragment(Settings settings)
{
    return "";
}

void input_flush()
{
    int c;
    while((c = getchar()) != '\n' && c != EOF) {}
}

void ask_settings(Settings &settings)
{
    puts("Configure Settings...");

    for(auto &[name, value] : settings) {
        printf("\n");
        while(true) {
            if(std::holds_alternative<bool>(value)) {
                printf("%s [1, 0]: ", name.c_str());

                int num = getchar() - '0';
                if(num == 1 || num == 0) {
                    value = bool(num);

                    break;
                }
            }
            else if(std::holds_alternative<Choices>(value)) {
                const auto &choices = std::get<Choices>(value);

                printf("%s\n", name.c_str());

                for(size_t i = 0; i < choices.size(); ++i) {
                    printf("    (%zu) %s\n", i, choices[i].c_str());
                }

                printf("[0..%zu]: ", choices.size() - 1);
                int num = getchar() - '0';

                if(size_t(num) < choices.size()) {
                    std::string option = choices[num];
                    value = option;

                    break;
                }
            }
            else if(std::holds_alternative<std::string>(value)) {
                printf("%s [\"?\"]: \n", name.c_str());

                char buf[512];
                if(fgets(buf, sizeof(buf), stdin)) {
                    buf[sizeof(buf) - 1] = '\0';
                    value = std::string(buf);

                    break;
                }

            }
            input_flush();
        }
        input_flush();
    }

    printf("\n\n\n");
}

void dump_settings(const Settings &settings)
{
    for(auto &[name, value] : settings) {
        if(auto *v = std::get_if<bool>(&value)) {
            printf("%s: %d\n", name.c_str(), int(*v));
        }
        if(auto *v = std::get_if<Choices>(&value)) {
            printf("%s: ...\n", name.c_str());

            for(const auto &choice : *v) {
                printf("\t%s\n", choice.c_str());
            }
        }
        if(auto *v = std::get_if<std::string>(&value)) {
            printf("%s: %s\n", name.c_str(), v->c_str());
        }
    }
}

void usage()
{
    static const char *help = "mkbuildconf [output path]";
    printf("\n%s\n", help);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        usage();

        return 1;
    }

    ask_settings(build_settings);
    dump_settings(build_settings);

    std::string make_fragment = generate_make_fragment(build_settings);
    printf("%s\n", make_fragment.c_str());

    return 0;
}
