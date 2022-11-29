#include <Object/id.h>

const uuids::uuid generateId(){
    std::random_device rd;
    auto seed_data = std::array<int, std::mt19937::state_size> {};
    std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::mt19937 generator(seq);
    uuids::uuid_random_generator gen{generator};
    return gen();
}

std::ostream & operator<<(std::ostream & os, Id const & value){
    os << uuids::to_string(value.id);
    return os;
}