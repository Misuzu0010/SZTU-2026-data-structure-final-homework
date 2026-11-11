#include <bits/stdc++.h>
#pragma GCC optimize("O3,unroll-loops")
using i64 = long long;
using namespace std;

struct network_state {

    int nodes_per_datacenter = 0; // 每个机房的节点数
    vector<int> rack_nodes; // 每个机架接入的网络节点
    vector<i64> edges; // 在线链路存非负时延，离线时按位取反，按输入顺序编号
    vector<vector<pair<int, int>>> adjacency; // 邻接表，每项为邻点和链路编号
    vector<unsigned char> node_online; // 节点是否通电
    array<array<i64, 30>, 30> distance_cache; // 机架间距离，-2 未算，-1 不可达
    vector<i64> distances; // Dijkstra 复用的节点距离表
    vector<pair<i64, int>> distance_heap; // Dijkstra 复用的小根堆
    vector<unsigned char> target_nodes; // 网络节点是否连接至少一个机架

    void initialize(int node_count, int datacenter_count, int rack_count) {
        nodes_per_datacenter = node_count / datacenter_count;
        rack_nodes.resize(rack_count);
        adjacency.resize(node_count);
        node_online.assign(node_count, 1);
        distances.resize(node_count);
        target_nodes.assign(node_count, 0);
        clear_distances();
    }

    void add_link(int from_node, int to_node, i64 latency) {
        int link_id = edges.size();
        edges.push_back(latency);
        adjacency[from_node].emplace_back(to_node, link_id);
        adjacency[to_node].emplace_back(from_node, link_id);
    }

    void clear_distances() {
        for (auto& row : distance_cache)
            row.fill(-2);
    }

    void set_datacenter_online(int datacenter_id, bool is_online) {
        int first_node = datacenter_id * nodes_per_datacenter;
        if (node_online[first_node] != is_online) {
            fill(node_online.begin() + first_node,
                 node_online.begin() + first_node + nodes_per_datacenter, is_online);
            clear_distances();
        }
    }

    void set_link_online(int link_id, bool is_online) {
        if ((edges[link_id] >= 0) != is_online) {
            edges[link_id] = ~edges[link_id];
            clear_distances();
        }
    }

    i64 distance(int from_rack, int to_rack) {
        i64& cached = distance_cache[from_rack][to_rack];
        return cached == -2 ? cached = compute_distances(from_rack, to_rack) : cached;
    }

    i64 compute_distances(int from_rack, int to_rack) {
        int source_node = rack_nodes[from_rack], destination_node = rack_nodes[to_rack];
        if (!node_online[source_node] || !node_online[destination_node])
            return -1;
        if (source_node == destination_node)
            return 0;
        fill(target_nodes.begin(), target_nodes.end(), 0);
        int targets_left = 0;
        for (int node_id : rack_nodes)
            if (!target_nodes[node_id]) {
                target_nodes[node_id] = 1;
                targets_left += node_online[node_id];
            }
        fill(distances.begin(), distances.end(), -1);
        distances[source_node] = 0;
        distance_heap.clear();
        distance_heap.emplace_back(0, source_node);
        while (!distance_heap.empty()) {
            pop_heap(distance_heap.begin(), distance_heap.end());
            auto [distance_so_far, node_id] = distance_heap.back();
            distance_so_far = -distance_so_far;
            distance_heap.pop_back();
            if (distance_so_far != distances[node_id])
                continue;
            if (target_nodes[node_id] && !--targets_left)
                break;
            for (auto [neighbor_id, link_id] : adjacency[node_id])
                if (node_online[neighbor_id] && edges[link_id] >= 0 &&
                    distance_so_far <= numeric_limits<i64>::max() - edges[link_id]) {
                    i64 next_distance = distance_so_far + edges[link_id];
                    if (distances[neighbor_id] < 0 || next_distance < distances[neighbor_id]) {
                        distances[neighbor_id] = next_distance;
                        distance_heap.emplace_back(-next_distance, neighbor_id);
                        push_heap(distance_heap.begin(), distance_heap.end());
                    }
                }
        }
        for (size_t rack_id = 0; rack_id < rack_nodes.size(); ++rack_id)
            distance_cache[rack_id][from_rack] = distance_cache[from_rack][rack_id] =
                distances[rack_nodes[rack_id]];
        return distance_cache[from_rack][to_rack];
    }
};

struct layer_cache {
    static const int sentinel = 400; // 循环链表的哨兵编号，也是镜像层数上限
    inline static long long layer_sizes[sentinel]{}; // 各镜像层的大小，所有缓存共用
    long long capacity = 0, used_bytes = 0; // 容量与当前占用量
    array<short, sentinel + 1> previous_layer{},
        next_layer{}; // 层的前驱、后继；前驱为 -1 表示不存在

    layer_cache() {
        clear();
    }

    bool contains(int layer_id) {
        return previous_layer[layer_id] >= 0;
    }

    void clear() {
        previous_layer.fill(-1);
        previous_layer[sentinel] = next_layer[sentinel] = sentinel;
        used_bytes = 0;
    }

    void unlink_layer(int layer_id) {
        next_layer[previous_layer[layer_id]] = next_layer[layer_id];
        previous_layer[next_layer[layer_id]] = previous_layer[layer_id];
        previous_layer[layer_id] = -1;
    }

    void touch_layer(int layer_id) {
        if (contains(layer_id)) {
            if (next_layer[layer_id] == sentinel)
                return;
            unlink_layer(layer_id);
        } else {
            if (layer_sizes[layer_id] > capacity) {
                if (next_layer[sentinel] != sentinel)
                    clear();
                return;
            }
            while (used_bytes > capacity - layer_sizes[layer_id]) {
                int oldest_layer = next_layer[sentinel];
                used_bytes -= layer_sizes[oldest_layer];
                unlink_layer(oldest_layer);
            }
            used_bytes += layer_sizes[layer_id];
        }
        previous_layer[layer_id] = previous_layer[sentinel];
        next_layer[layer_id] = sentinel;
        next_layer[previous_layer[sentinel]] = layer_id;
        previous_layer[sentinel] = layer_id;
    }
};

struct pull_amounts {
    long long registry_bytes, rack_bytes;
}; // 仓库与机架的拉取量

inline pull_amounts pull_image_layers(layer_cache& machine_cache, layer_cache& rack_cache,
                                      vector<int>& layers) {
    pull_amounts amounts{};
    for (int layer_id : layers) {
        if (!machine_cache.contains(layer_id)) {
            (rack_cache.contains(layer_id) ? amounts.rack_bytes : amounts.registry_bytes) +=
                layer_cache::layer_sizes[layer_id];
            rack_cache.touch_layer(layer_id);
        }
        machine_cache.touch_layer(layer_id);
    }
    return amounts;
}

inline pull_amounts preview_image_pull(layer_cache& machine_cache, layer_cache& rack_cache,
                                       vector<int>& layers) {
    for (int layer_id : layers)
        if (!machine_cache.contains(layer_id)) {
            layer_cache machine_copy = machine_cache, rack_copy = rack_cache;
            return pull_image_layers(machine_copy, rack_copy, layers);
        }
    return {};
}

struct log_tracker {

    struct pattern_node {
        array<int, 8> next_state{}; // 字母转移，构建后补全失败转移
        i64 weight_sum = 0; // 在该状态结尾的所有告警模式的权重和
        i64 high_weight_match = 0; // 构建前为最高单模式权重，构建后为是否达到固定告警阈值
    };

    vector<pattern_node> patterns{1}; // 告警模式的 AC 自动机，0 是根
    array<array<uint64_t, 256>, 17> decode_table{}; // 低32位为加一后的四位符号流，高位为赫夫曼终态
    array<array<int, 2>, 17> huffman_children{}; // 赫夫曼树的左右子结点
    int window_slot = 0; // 本 tick 对应的窗口桶
    vector<i64> alarm_scores; // 每台机器当前窗口的告警值
    vector<vector<i64>> window; // 按 tick 取模记录每台机器的告警分

    void initialize(int machine_count, int window_size, array<i64, 9>& frequencies) {
        alarm_scores.assign(machine_count, 0);
        window.assign(window_size, vector<i64>(machine_count));
        using heap_entry = pair<i64, int>;
        priority_queue<heap_entry, vector<heap_entry>, greater<heap_entry>> frequency_heap;
        for (int index = 0; index < 9; ++index)
            frequency_heap.emplace(frequencies[index], index);
        for (int index = 9; index < 17; ++index) {
            auto [left_weight, left_child] = frequency_heap.top();
            frequency_heap.pop();
            auto [right_weight, right_child] = frequency_heap.top();
            frequency_heap.pop();
            huffman_children[index] = {left_child, right_child};
            frequency_heap.emplace(left_weight + right_weight, index);
        }
        for (int start_node = 9; start_node < 17; ++start_node)
            for (int byte_value = 0; byte_value < 256; ++byte_value) {
                auto& entry = decode_table[start_node][byte_value];
                int state = start_node, symbol_count = 0;
                for (int index = 7; index >= 0; --index) {
                    state = huffman_children[state][byte_value >> index & 1];
                    if (state < 9) {
                        entry |= uint64_t(state + 1) << (4 * symbol_count++);
                        state = 16;
                    }
                }
                entry |= uint64_t(state) << 32;
            }
    }

    void add_pattern(string& pattern_text, i64 weight) {
        int state = 0;
        for (char letter : pattern_text) {
            int symbol = letter - 'a', next = patterns[state].next_state[symbol];
            if (!next) {
                next = patterns.size();
                patterns[state].next_state[symbol] = next;
                patterns.emplace_back();
            }
            state = next;
        }
        patterns[state].weight_sum += weight;
        patterns[state].high_weight_match = max(patterns[state].high_weight_match, weight);
    }

    void build_matcher(i64 threshold) {
        vector<int> failure_links(patterns.size()), order;
        order.reserve(patterns.size());
        for (int next : patterns[0].next_state)
            if (next)
                order.push_back(next);
        for (size_t index = 0; index < order.size(); ++index) {
            int state = order[index], fallback = failure_links[state];
            patterns[state].weight_sum += patterns[fallback].weight_sum;
            patterns[state].high_weight_match = patterns[state].high_weight_match >= threshold ||
                                                patterns[fallback].high_weight_match;
            for (int symbol = 0; symbol < 8; ++symbol) {
                int& next = patterns[state].next_state[symbol];
                if (next) {
                    failure_links[next] = patterns[fallback].next_state[symbol];
                    order.push_back(next);
                } else
                    next = patterns[fallback].next_state[symbol];
            }
        }
    }

    void advance_window(int tick) {
        window_slot = tick % (int)window.size();
        auto& row = window[window_slot];
        for (size_t machine_id = 0; machine_id < row.size(); ++machine_id) {
            alarm_scores[machine_id] -= row[machine_id];
            row[machine_id] = 0;
        }
    }

    static int hex_digit(char digit) {
        return digit <= '9' ? digit - '0' : (digit | 32) - 'a' + 10;
    }

    i64 add_log(int machine_id, int bit_count, string& hex_text) {
        int decode_state = 16, match_state = 0, hex_offset = 0;
        i64 score = 0;
        bool has_high_weight = 0;
        auto feed = [&](int symbol) {
            if (symbol == 8)
                match_state = 0;
            else {
                match_state = patterns[match_state].next_state[symbol];
                score += patterns[match_state].weight_sum;
                has_high_weight |= patterns[match_state].high_weight_match;
            }
        };
        for (; bit_count >= 8; bit_count -= 8, hex_offset += 2) {
            auto entry = decode_table[decode_state][hex_digit(hex_text[hex_offset]) * 16 +
                                                    hex_digit(hex_text[hex_offset + 1])];
            decode_state = entry >> 32;
            for (unsigned symbols = entry; symbols; symbols >>= 4)
                feed((symbols & 15) - 1);
        }
        for (int bit_index = 0; bit_index < bit_count; ++bit_index) {
            decode_state =
                huffman_children[decode_state][hex_digit(hex_text[hex_offset + bit_index / 4]) >>
                                                   (3 - bit_index % 4) &
                                               1];
            if (decode_state < 9) {
                feed(decode_state);
                decode_state = 16;
            }
        }
        window[window_slot][machine_id] += score;
        alarm_scores[machine_id] += score;
        return has_high_weight;
    }
};

#include <unistd.h>

using i128 = __int128_t;
const int command_limit = 20000;

static i64 ceil_div(i64 numerator, i64 denominator) {
    return numerator < denominator ? (numerator != 0)
                                   : numerator / denominator + (numerator % denominator != 0);
}

struct buffered_io {
    char input_buffer[1 << 16];
    int input_offset, input_size; // 管道输入块与读写位置
    string output_buffer; // 当前 tick 的完整回复

    int read_char() {
        if (input_offset == input_size) {
            input_size = ::read(0, input_buffer, sizeof input_buffer);
            input_offset = 0;
        }
        return input_buffer[input_offset++];
    }

    int skip_whitespace() {
        int character;
        do
            character = read_char();
        while (character <= 32);
        return character;
    }

    void read_word(string& word) {
        word.clear();
        int character = skip_whitespace();
        for (; character > 32; character = read_char())
            word += character;
    }

    i64 read_int() {
        int character = skip_whitespace();
        bool is_negative = character == '-';
        if (is_negative)
            character = read_char();
        uint64_t value = 0;
        for (; character > 32; character = read_char())
            value = value * 10 + character - '0';
        return is_negative ? -value : value;
    }

    template <class value_type>
    void write_int(value_type value) {
        char digits[48];
        int digit_count = 0;
        if (value < 0) {
            output_buffer += '-';
            value = -value;
        }
        do {
            digits[digit_count++] = '0' + value % 10;
            value /= 10;
        } while (value);
        while (digit_count)
            output_buffer += digits[--digit_count];
    }

    void flush() {
        output_buffer += "END\n";
        fwrite(output_buffer.data(), 1, output_buffer.size(), stdout);
        fflush(stdout);
        output_buffer.clear();
    }
} io;

struct tenant_state {
    int parent_id = -1, sample_count; // 父租户、真实时长样本数
    i64 quota, used_cores; // 配额、子树已用核数
    i128 bill_offset, billing_rate; // 仿射账单常数、子树每 tick 运行费
    double duration_ratio_sum, duration_factor = 1.2; // 时长倍率样本和、本轮预测倍率
    vector<int> ancestor_path; // 从本租户到根的路径
};

struct machine_state {
    int rack_id, last_alert_tick = -100,
                 last_failure_tick = -100; // 所在机架、最近高权重告警和故障的 tick
    i64 free_cores, free_memory; // 当前空闲核数和内存
    i64 total_cores, total_memory; // 单机总容量，用于判断作业是否永久无处运行
    bool online = 1; // 是否正常
    layer_cache cache; // 机器层缓存
    vector<int> running_jobs; // 运行中作业的下标
};

struct rack_state {
    int datacenter_id; // 所在机房
    layer_cache cache; // 机架层缓存
};

struct image_spec {
    vector<int> layers; // 从底到顶的镜像层
    i64 total_bytes; // 镜像层大小之和
    array<int, 3> sample_layers{}; // 粗筛机器时查看的三层
};

struct input_data {
    i64 total_bytes, longest_send_time; // 同源输入的总大小、最长串行传输耗时
    int source_rack; // 输入源机架
};

struct job_state {
    int job_id, workflow_index, image_id,
        external_source_rack = -1; // 外部编号、所属工作流、镜像、外部输入机架
    int predecessors_begin, predecessors_end, successors_begin, successors_end,
        unfinished_predecessors; // 前驱和后继的半开区间、未完成前驱数
    int list_position = -1, machine_id = -1,
        start_tick; // 就绪/运行表位置、机器、输出机架、启动 tick
    int retry_environment = -1; // 未投入工作流上次放置失败时的环境版本
    unsigned status : 2, compatible_racks : 30; // 作业状态、已证在线可容纳的机架位集
    i64 required_cores, required_memory, estimated_duration, external_input_bytes,
        output_bytes; // 核数、内存、估计时长、输入大小、输出大小
    i64 critical_tail, rough_tail, earliest_start,
        prepare_time; // 精确尾长、含准备的预测尾长、最早起点、实际准备时长
    vector<input_data> delivered_inputs; // 已完成前驱按实际机架累计的字节数与最长串行传输期
    double planned_tail = -1, planned_cost; // 计划中的后继关键路径、当前作业的预估剩余费用
    uint64_t plan_predecessors, running_predecessors; // 静态约简前驱、原前驱列表中正在运行的位集
};

struct workflow_state {
    i64 max_unfinished_cores; // 未完成作业最大核数，负值表示完成事件后需要重算
    int tenant_id, jobs_begin, jobs_end, deadline, grace_period, last_profitable_tick,
        finished_jobs; // 租户、作业区间、截止、宽限、最后可收益 tick、完成数
    int audited_finished_jobs = -1, schedule_tick; // 最后完成 tick、松弛缓存版本、预测版本
    i64 value, critical_path, unfinished_cores; // 初始价值、审计关键路径、未完成作业的核数和
    double estimated_work, frontier_tail, estimated_work_done, remaining_time, remaining_cost,
        priority; // 剩余估计核时、临界路径、运行中预计核时、完工估时、预计费用、优先级
    bool has_started; // 是否已投入资源
    int reroute_tick = -1; // 本流程最近一次尝试救急重选的 tick
    int preferred_rack = -1, plan_tick = -100,
        planned_finished_jobs = -1; // 计划锚定机架、计划tick、完成状态版本
    double plan_cost, plan_duration; // 整个未完成部分的预计费用、关键路径
    int detailed_plan_tick = -1,
        detailed_plan_environment = -1; // 最后完整恢复的精确规划对应 tick 与环境版本
};

struct image_hint {
    int time_bucket = -1; // 估价对应的七 tick 规划周期
    int machine_id; // 同时考虑拉取费与准备耗时后选中的机器
    pull_amounts pull_estimate; // 同一台机器预计从仓库和机架拉取的字节数
};

struct transfer_estimate {
    i64 transfer_time = -1;
    i128 transfer_fee; // 传输耗时（负数不可达）、传输费
};

using candidate_entry = pair<double, int>; // 负优先级与作业下标，或机器粗排分与机器下标

struct scheduler {
    int total_ticks, datacenter_count, node_count, rack_count, machine_count, tenant_count,
        layer_count, image_count, pattern_count, alarm_window,
        link_count; // 初始化规模，含 tick 总数
    int current_tick, commands_sent, duration_sample_count; // 当前 tick、指令数、完成样本数
    int reroute_attempts; // 本 tick 的救急重选次数，限制最坏规模规划开销
    int environment_version; // 会改变资源、缓存、估时或冷启动协作机会的环境版本
    bool used_cache_investment; // 本 tick 是否已为共享镜像做过一次冷启动投资
    bool compatibility_cache_dirty; // 本 tick 有机器故障，需要在规划前清除容量正命中
    i64 registry_bandwidth, rack_bandwidth, network_bandwidth, registry_unit_fee, network_unit_fee,
        max_pattern_weight; // 带宽、费用、最大告警模式权重
    i64 min_cpu_price; // 全场最低 CPU 单价
    double duration_ratio_sum; // 全场真实时长倍率样本和
    vector<i64> cpu_prices; // 机房 CPU 单价
    vector<rack_state> racks;
    vector<machine_state> machines; // 机架和机器状态
    vector<tenant_state> tenants;
    vector<image_spec> images; // 租户和镜像
    vector<pair<int, i64>> quota_history; // 配额的撤销记录
    vector<job_state> jobs;
    vector<workflow_state> workflows; // 按工作流连续存放的作业、工作流
    vector<int> predecessors, successors, ready_jobs, online_machines,
        active_workflows; // 前驱、后继、就绪表、在线机器、待更新的工作流
    vector<i64> quota_headroom, quota_limits; // 本轮的租户路径剩余配额与总配额
    vector<candidate_entry> job_candidates; // 候选优先级
    double plan_stages[2][40], plan_costs[2][40]; // 规划双缓冲：当前试算和胜出方案的阶段耗时、费用
    int plan_buffer = 0; // 正在写入的规划槽
    using plan_snapshot =
        array<pair<double, double>, 40>; // 单条可调度工作流的后继尾长与作业费用缓冲区
    vector<vector<int>> machines_by_rack; // 各机架机器下标
    vector<double> tenant_workload; // 各租户子树已接纳工作流的剩余占核时
    array<i64, 30> rack_free_cores{}, rack_free_memory{}; // 每机架在本轮开始时的最大单机空闲资源
    array<double, 30> rack_workload{}, inverse_rack_cores; // 机架的剩余核时、有效在线核数倒数
    array<i64, 30> rack_online_cores{}, rack_max_cores{},
        rack_max_memory{}; // 机架在线总核数、单机额定资源上界
    array<int, 30> representative_machine; // 每机架的代表缓存机器，按 tick 重建
    unordered_map<int, int> job_index_by_id{65536}; // 开放寻址表将九位作业编号映射到连续下标
    vector<image_hint> image_hints; // 按机架与镜像缓存本轮乐观暖度
    network_state network;
    log_tracker logs; // 动态最短路和告警统计
    string operation, token; // 协议关键字与临时字符串

    static bool fits_machine_capacity(machine_state& machine, job_state& job) {
        return machine.total_cores >= job.required_cores &&
               machine.total_memory >= job.required_memory;
    }

    void initialize() {
        total_ticks = io.read_int();
        datacenter_count = io.read_int();
        node_count = io.read_int();
        rack_count = io.read_int();
        machine_count = io.read_int();
        tenant_count = io.read_int();
        layer_count = io.read_int();
        image_count = io.read_int();
        pattern_count = io.read_int();
        alarm_window = io.read_int();
        link_count = io.read_int();
        registry_bandwidth = io.read_int();
        rack_bandwidth = io.read_int();
        network_bandwidth = io.read_int();
        registry_unit_fee = io.read_int();
        network_unit_fee = io.read_int();
        cpu_prices.resize(datacenter_count);
        for (auto& unit_price : cpu_prices)
            unit_price = io.read_int();
        min_cpu_price = *min_element(cpu_prices.begin(), cpu_prices.end());
        network.initialize(node_count, datacenter_count, rack_count);
        racks.resize(rack_count);
        machines.resize(machine_count);
        tenants.resize(tenant_count);
        images.resize(image_count);
        for (int rack_id = 0; rack_id < rack_count; ++rack_id) {
            racks[rack_id].datacenter_id = io.read_int();
            network.rack_nodes[rack_id] = io.read_int();
            racks[rack_id].cache.capacity = io.read_int();
        }
        for (auto& machine : machines) {
            machine.rack_id = io.read_int();
            machine.free_cores = machine.total_cores = io.read_int();
            machine.free_memory = machine.total_memory = io.read_int();
            machine.cache.capacity = io.read_int();
        }
        for (int link_id = 0; link_id < link_count; ++link_id) {
            int node_id = io.read_int(), neighbor_id = io.read_int();
            i64 weight = io.read_int();
            network.add_link(node_id, neighbor_id, weight);
        }
        for (auto& tenant : tenants) {
            tenant.parent_id = io.read_int();
            tenant.quota = io.read_int();
        }
        for (int entry = 0; entry < tenant_count; ++entry)
            for (int node_id = entry; node_id >= 0; node_id = tenants[node_id].parent_id)
                tenants[entry].ancestor_path.push_back(node_id);
        for (int layer_id = 0; layer_id < layer_count; ++layer_id)
            layer_cache::layer_sizes[layer_id] = io.read_int();
        for (auto& image : images) {
            image.layers.resize(io.read_int());
            int largest_layer_index = 0;
            for (int layer_index = 0; layer_index < (int)image.layers.size(); ++layer_index) {
                int layer_id = image.layers[layer_index] = io.read_int();
                image.total_bytes += layer_cache::layer_sizes[layer_id];
                if (layer_cache::layer_sizes[layer_id] >
                    layer_cache::layer_sizes[image.layers[largest_layer_index]])
                    largest_layer_index = layer_index;
            }
            int layer_total = image.layers.size();
            if (layer_total)
                image.sample_layers = {
                    image.layers[0],
                    image.layers[largest_layer_index == 0 ? layer_total / 2 : largest_layer_index],
                    image.layers[layer_total - 1]};
        }
        array<i64, 9> frequencies;
        for (auto& frequency : frequencies)
            frequency = io.read_int();
        logs.initialize(machine_count, alarm_window, frequencies);
        for (int pattern_id = 0; pattern_id < pattern_count; ++pattern_id) {
            io.read_word(token);
            i64 weight = io.read_int();
            logs.add_pattern(token, weight);
            max_pattern_weight = max(max_pattern_weight, weight);
        }
        logs.build_matcher(max<i64>(8, max_pattern_weight / 6));
        jobs.reserve(60000);
        image_hints.resize(rack_count * image_count);
        quota_headroom.resize(tenant_count);
        quota_limits.resize(tenant_count);
        tenant_workload.resize(tenant_count);
        machines_by_rack.resize(rack_count);
        for (int machine_id = 0; machine_id < machine_count; ++machine_id)
            machines_by_rack[machines[machine_id].rack_id].push_back(machine_id);
    }

    void push_ready(int job_index) {
        jobs[job_index].list_position = ready_jobs.size();
        ready_jobs.push_back(job_index);
    }

    void pop_ready(int job_index) {
        int position = jobs[job_index].list_position;
        ready_jobs[position] = ready_jobs.back();
        jobs[ready_jobs[position]].list_position = position;
        ready_jobs.pop_back();
        jobs[job_index].list_position = -1;
    }

    void read_workflow() {
        io.read_int();
        workflow_state flow{};
        flow.tenant_id = io.read_int();
        flow.value = io.read_int();
        flow.deadline = io.read_int();
        int job_count = io.read_int();
        flow.grace_period = max(5, (flow.deadline - current_tick) / 3);
        flow.last_profitable_tick =
            min<i64>(total_ticks, (i64)flow.deadline + flow.grace_period - 1);
        flow.jobs_begin = jobs.size();
        flow.jobs_end = flow.jobs_begin + job_count;
        int workflow_index = workflows.size();
        jobs.resize(flow.jobs_end);
        for (int job_index = flow.jobs_begin; job_index < flow.jobs_end; ++job_index) {
            auto& job = jobs[job_index];
            job.job_id = io.read_int();
            job_index_by_id[job.job_id] = job_index;
            job.workflow_index = workflow_index;
            job.required_cores = io.read_int();
            job.required_memory = io.read_int();
            job.estimated_duration = io.read_int();
            job.image_id = io.read_int();
            job.external_source_rack = io.read_int();
            job.external_input_bytes = io.read_int();
            job.output_bytes = io.read_int();
            job.unfinished_predecessors = io.read_int();
            job.predecessors_begin = predecessors.size();
            for (int edge_index = 0; edge_index < job.unfinished_predecessors; ++edge_index) {
                int predecessor_index = job_index_by_id[io.read_int()];
                predecessors.push_back(predecessor_index);
                ++jobs[predecessor_index].successors_end;
            }
            job.predecessors_end = predecessors.size();
            flow.estimated_work += (double)job.required_cores * job.estimated_duration;
            flow.unfinished_cores += job.required_cores;
            flow.max_unfinished_cores = max(flow.max_unfinished_cores, job.required_cores);
            if (!job.unfinished_predecessors)
                push_ready(job_index);
            if (none_of(machines.begin(), machines.end(), [&](machine_state& machine) {
                    return fits_machine_capacity(machine, job);
                }))
                flow.last_profitable_tick = 0;
        }
        int total = successors.size();
        for (int job_index = flow.jobs_begin; job_index < flow.jobs_end; ++job_index) {
            auto& job = jobs[job_index];
            total += job.successors_end;
            job.successors_begin = job.successors_end = total;
        }
        successors.resize(total);
        for (int job_index = flow.jobs_end; job_index-- > flow.jobs_begin;) {
            auto& job = jobs[job_index];
            job.critical_tail += job.estimated_duration;
            job.rough_tail += job.estimated_duration + 1;
            for (int edge_index = job.predecessors_begin; edge_index < job.predecessors_end;
                 ++edge_index) {
                auto& predecessor_index = jobs[predecessors[edge_index]];
                int position = --predecessor_index.successors_begin;
                successors[position] = job_index;
                predecessor_index.critical_tail =
                    max(predecessor_index.critical_tail, job.critical_tail);
                predecessor_index.rough_tail = max(predecessor_index.rough_tail, job.rough_tail);
            }
        }
        if (job_count <= 40) {
            uint64_t ancestors[40]{};
            for (int job_index = flow.jobs_begin; job_index < flow.jobs_end; ++job_index) {
                auto& job = jobs[job_index];
                uint64_t pending_predecessors = 0;
                for (int edge_index = job.predecessors_begin; edge_index < job.predecessors_end;
                     ++edge_index) {
                    int predecessor_index = predecessors[edge_index] - flow.jobs_begin;
                    pending_predecessors |= 1ULL << predecessor_index;
                    ancestors[job_index - flow.jobs_begin] |=
                        ancestors[predecessor_index] | (1ULL << predecessor_index);
                }
                while (pending_predecessors) {
                    int predecessor_index = 63 - __builtin_clzll(pending_predecessors);
                    job.plan_predecessors |= 1ULL << predecessor_index;
                    pending_predecessors &=
                        ~(ancestors[predecessor_index] | (1ULL << predecessor_index));
                }
            }
        }
        workflows.push_back(flow);
    }

    i64 remaining_quota(int tenant_id) {
        i64 remaining = LLONG_MAX;
        for (int ancestor_id : tenants[tenant_id].ancestor_path)
            remaining =
                min(remaining, tenants[ancestor_id].quota - tenants[ancestor_id].used_cores);
        return remaining;
    }

    void update_account(job_state& job, int machine_id, int sign, i128 immediate = 0) {
        i128 rate_change = (i128)job.required_cores *
                           cpu_prices[racks[machines[machine_id].rack_id].datacenter_id] * sign;
        for (int ancestor_id : tenants[workflows[job.workflow_index].tenant_id].ancestor_path) {
            auto& tenant = tenants[ancestor_id];
            tenant.used_cores += job.required_cores * sign;
            tenant.billing_rate += rate_change;
            tenant.bill_offset += immediate - rate_change * current_tick;
        }
    }

    void update_running_predecessors(int job_index) {
        for (int edge_index = jobs[job_index].successors_begin;
             edge_index < jobs[job_index].successors_end; ++edge_index) {
            auto& successor = jobs[successors[edge_index]];
            if (successor.predecessors_end - successor.predecessors_begin <= 40)
                successor.running_predecessors ^=
                    1ULL << (find(predecessors.begin() + successor.predecessors_begin,
                                  predecessors.begin() + successor.predecessors_end, job_index) -
                             predecessors.begin() - successor.predecessors_begin);
        }
    }

    void release_job(int job_index, bool done) {
        update_running_predecessors(job_index);
        auto& job = jobs[job_index];
        auto& machine = machines[job.machine_id];
        auto& flow = workflows[job.workflow_index];
        update_account(job, job.machine_id, -1);
        machine.free_cores += job.required_cores;
        machine.free_memory += job.required_memory;
        machine.running_jobs[job.list_position] = machine.running_jobs.back();
        jobs[machine.running_jobs[job.list_position]].list_position = job.list_position;
        machine.running_jobs.pop_back();
        job.list_position = job.machine_id = -1;
        if (!done) {
            job.status = 0;
            push_ready(job_index);
            return;
        }
        job.status = 2;
        ++flow.finished_jobs;
        flow.estimated_work -= (double)job.required_cores * job.estimated_duration;
        flow.unfinished_cores -= job.required_cores;
        if (job.required_cores == flow.max_unfinished_cores)
            flow.max_unfinished_cores = -1;
        if (job.estimated_duration) {
            double ratio = clamp((current_tick - job.start_tick - job.prepare_time) /
                                     (double)job.estimated_duration,
                                 .25, 4.);
            auto& tenant = tenants[flow.tenant_id];
            tenant.duration_ratio_sum += ratio;
            ++tenant.sample_count;
            duration_ratio_sum += ratio;
            ++duration_sample_count;
        }
        i64 send_time = ceil_div(job.output_bytes, network_bandwidth);
        for (int edge_index = job.successors_begin; edge_index < job.successors_end; ++edge_index) {
            int successor_index = successors[edge_index];
            auto& next = jobs[successor_index];
            {
                auto& inputs = next.delivered_inputs;
                auto input_position =
                    find_if(inputs.begin(), inputs.end(), [&](input_data& tenant) {
                        return tenant.source_rack == machine.rack_id;
                    });
                if (input_position == inputs.end())
                    inputs.push_back({job.output_bytes, send_time, machine.rack_id});
                else {
                    input_position->total_bytes += job.output_bytes;
                    input_position->longest_send_time =
                        max(input_position->longest_send_time, send_time);
                }
            }
            if (!--next.unfinished_predecessors)
                push_ready(successor_index);
        }
        vector<input_data>().swap(job.delivered_inputs);
    }

    i64 job_slack(int job_index) {
        auto& job = jobs[job_index];
        if (job.status == 2)
            return 0;
        auto& flow = workflows[job.workflow_index];
        if (flow.audited_finished_jobs != flow.finished_jobs) {
            flow.audited_finished_jobs = flow.finished_jobs;
            flow.critical_path = 0;
            for (int job_cursor = flow.jobs_begin; job_cursor < flow.jobs_end; ++job_cursor) {
                auto& current_job = jobs[job_cursor];
                if (current_job.status == 2)
                    continue;
                current_job.earliest_start = 0;
                for (int edge_index = current_job.predecessors_begin;
                     edge_index < current_job.predecessors_end; ++edge_index) {
                    auto& predecessor = jobs[predecessors[edge_index]];
                    if (predecessor.status != 2)
                        current_job.earliest_start =
                            max(current_job.earliest_start,
                                predecessor.earliest_start + predecessor.estimated_duration);
                }
                flow.critical_path = max(flow.critical_path, current_job.critical_tail);
            }
        }
        return flow.critical_path - job.earliest_start - job.critical_tail;
    }

    void handle_event() {
        io.read_word(operation);
        if (operation != "LOG")
            ++environment_version;
        if (operation == "WF") {
            read_workflow();
            return;
        }
        int event_id = io.read_int();
        if (operation == "DONE")
            release_job(job_index_by_id[event_id], 1);
        else if (operation == "FAIL" || operation == "UP") {
            auto& machine = machines[event_id];
            machine.online = operation == "UP";
            machine.cache.clear();
            if (!machine.online) {
                compatibility_cache_dirty = 1;
                machine.last_failure_tick = current_tick;
                machine.last_alert_tick = -100;
                while (!machine.running_jobs.empty())
                    release_job(machine.running_jobs.back(), 0);
            }
        } else if (operation == "DCDOWN" || operation == "DCUP") {
            network.set_datacenter_online(event_id, operation == "DCUP");
        } else if (operation == "LDOWN" || operation == "LUP") {
            network.set_link_online(event_id, operation == "LUP");
        } else if (operation == "QUOTA") {
            quota_history.emplace_back(event_id, tenants[event_id].quota);
            tenants[event_id].quota = io.read_int();
        } else if (operation == "UNDO") {
            while (event_id--) {
                auto [tenant_id, old_quota] = quota_history.back();
                quota_history.pop_back();
                tenants[tenant_id].quota = old_quota;
            }
        } else {
            int bit_count = io.read_int();
            io.read_word(token);
            if (logs.add_log(event_id, bit_count, token))
                machines[event_id].last_alert_tick = current_tick;
        }
    }

    void answer_queries(int query_count) {
        io.output_buffer += "ANS";
        while (query_count--) {
            io.read_word(operation);
            int query_id = io.read_int();
            i128 answer = 0;
            if (operation == "BILL")
                answer =
                    tenants[query_id].bill_offset + tenants[query_id].billing_rate * current_tick;
            else if (operation == "HEAD")
                answer = remaining_quota(query_id);
            else if (operation == "ROUTE")
                answer = network.distance(query_id, io.read_int());
            else if (operation == "ALARM")
                answer = logs.alarm_scores[query_id];
            else if (operation == "SLACK")
                answer = job_slack(job_index_by_id[query_id]);
            else if (operation == "CACHE")
                answer = machines[query_id].cache.used_bytes;
            else
                answer = racks[query_id].cache.used_bytes;
            io.output_buffer += ' ';
            io.write_int(answer);
        }
        io.output_buffer += '\n';
    }

    void emit_command(const char* name, int id, int machine_id = -1) {
        io.output_buffer += name;
        io.write_int(id);
        if (machine_id >= 0) {
            io.output_buffer += ' ';
            io.write_int(machine_id);
        }
        io.output_buffer += '\n';
        ++commands_sent;
    }

    double reward_at(workflow_state& flow, double finish_time) {
        if (finish_time > total_ticks)
            return 0;
        return (double)flow.value *
               clamp((flow.deadline + double(flow.grace_period) - finish_time) / flow.grace_period,
                     0., 1.);
    }

    double estimate_reward(workflow_state& flow, double elapsed_time) {
        return .72 * reward_at(flow, current_tick + elapsed_time) +
               .28 * reward_at(flow, current_tick + max(1., .77 * elapsed_time));
    }

    static double estimate_tail(job_state& job, double duration_factor) {
        return job.planned_tail >= 0
                   ? job.planned_tail
                   : (job.critical_tail - job.estimated_duration) * duration_factor +
                         (job.rough_tail - job.critical_tail - 1);
    }

    double estimate_duration(job_state& job) {
        double duration =
            max(1., job.estimated_duration *
                        tenants[workflows[job.workflow_index].tenant_id].duration_factor);
        if (job.status != 1)
            return duration;
        double elapsed = current_tick - job.start_tick - job.prepare_time;
        return elapsed < 0 ? duration - elapsed : max({1., duration - elapsed, .25 * duration});
    }

    double estimate_remaining_work(workflow_state& flow) {
        return flow.estimated_work * tenants[flow.tenant_id].duration_factor +
               flow.unfinished_cores;
    }

    i64 image_prepare_time(pull_amounts amounts) {
        return ceil_div(amounts.registry_bytes, registry_bandwidth) +
               ceil_div(amounts.rack_bytes, rack_bandwidth);
    }

    void swap_plan_snapshot(workflow_state& flow, plan_snapshot& snapshot) {
        for (int job_index = flow.jobs_begin; job_index < flow.jobs_end; ++job_index) {
            swap(jobs[job_index].planned_tail, snapshot[job_index - flow.jobs_begin].first);
            swap(jobs[job_index].planned_cost, snapshot[job_index - flow.jobs_begin].second);
        }
    }

    void update_plan_tails(workflow_state& flow) {
        for (int job_index = flow.jobs_begin; job_index < flow.jobs_end; ++job_index)
            jobs[job_index].planned_tail = 0;
        for (int job_index = flow.jobs_end; job_index-- > flow.jobs_begin;) {
            auto& job = jobs[job_index];
            job.planned_cost = plan_costs[plan_buffer][job_index - flow.jobs_begin];
            for (uint64_t mask = job.plan_predecessors; mask; mask &= mask - 1) {
                auto& predecessor = jobs[flow.jobs_begin + __builtin_ctzll(mask)];
                predecessor.planned_tail =
                    max(predecessor.planned_tail,
                        plan_stages[plan_buffer][job_index - flow.jobs_begin] + job.planned_tail);
            }
        }
    }

    bool plan_on_rack(workflow_state& flow, int rack_id, bool calculate_tails = 1,
                      int machine = -1) {
        if (machine < 0)
            machine = representative_machine[rack_id];
        if (machine < 0 || !network.node_online[network.rack_nodes[rack_id]])
            return 0;
        layer_cache machine_cache = machines[machine].cache, rack_cache = racks[rack_id].cache;
        double finish[40];
        double span = 0, cost = 0, work = 0, unit = cpu_prices[racks[rack_id].datacenter_id];
        int last_image_id = -1;
        for (int job_index = flow.jobs_begin; job_index < flow.jobs_end; ++job_index) {
            int job_offset = job_index - flow.jobs_begin;
            auto& job = jobs[job_index];
            double& phase = plan_stages[plan_buffer][job_offset];
            finish[job_offset] = phase = plan_costs[plan_buffer][job_offset] = 0;
            if (job.status == 2)
                continue;
            double duration = estimate_duration(job), prepare_time = 0, charge = 0, begin = 0;
            if (job.status == 1) {
                finish[job_offset] = phase = duration;
                charge = duration * job.required_cores *
                         cpu_prices[racks[machines[job.machine_id].rack_id].datacenter_id];
            } else {
                if (!(job.compatible_racks >> rack_id & 1)) {
                    if (none_of(machines_by_rack[rack_id].begin(), machines_by_rack[rack_id].end(),
                                [&](int machine_id) {
                                    return machines[machine_id].online &&
                                           fits_machine_capacity(machines[machine_id], job);
                                }))
                        return 0;
                    job.compatible_racks |= 1u << rack_id;
                }
                double transfer = 0;
                auto input = [&](int source, i64 bytes) {
                    if (source == rack_id)
                        return 1;
                    i64 distance = network.distance(source, rack_id);
                    if (distance < 0)
                        return 0;
                    prepare_time =
                        max(prepare_time, (double)(distance + ceil_div(bytes, network_bandwidth)));
                    transfer += (double)bytes * distance * network_unit_fee;
                    return 1;
                };
                if (job.external_source_rack >= 0 &&
                    !input(job.external_source_rack, job.external_input_bytes))
                    return 0;
                for (input_data& input_item : job.delivered_inputs)
                    if (input_item.source_rack != rack_id) {
                        i64 distance = network.distance(input_item.source_rack, rack_id);
                        if (distance < 0)
                            return 0;
                        prepare_time =
                            max(prepare_time, (double)(distance + input_item.longest_send_time));
                        transfer += (double)input_item.total_bytes * distance * network_unit_fee;
                    }
                for (uint64_t mask = job.plan_predecessors; mask; mask &= mask - 1)
                    begin = max(begin, finish[__builtin_ctzll(mask)]);
                for (uint64_t mask = job.running_predecessors; mask; mask &= mask - 1) {
                    auto& predecessor =
                        jobs[predecessors[job.predecessors_begin + __builtin_ctzll(mask)]];
                    if (!input(machines[predecessor.machine_id].rack_id, predecessor.output_bytes))
                        return 0;
                }
                pull_amounts amounts{};
                if (job.image_id != last_image_id)
                    amounts =
                        pull_image_layers(machine_cache, rack_cache, images[job.image_id].layers);
                last_image_id =
                    images[job.image_id].total_bytes <= machine_cache.capacity ? job.image_id : -1;
                if (amounts.registry_bytes || amounts.rack_bytes)
                    prepare_time = max(prepare_time, (double)image_prepare_time(amounts));
                charge = transfer + (double)amounts.registry_bytes * registry_unit_fee +
                         (prepare_time + duration) * job.required_cores * unit;
                phase = prepare_time + duration;
                finish[job_offset] = begin + phase;
            }
            plan_costs[plan_buffer][job_offset] = charge;
            cost += charge;
            work += phase * job.required_cores;
            span = max(span, finish[job_offset]);
        }
        span =
            max(span,
                work / max<i64>(1, min(quota_limits[flow.tenant_id], rack_online_cores[rack_id])));
        double own =
            flow.has_started && rack_id == flow.preferred_rack ? estimate_remaining_work(flow) : 0;
        span +=
            .25 * max(0., rack_workload[rack_id] - own) / max<i64>(1, rack_online_cores[rack_id]);
        flow.plan_cost = cost;
        flow.plan_duration = span;
        if (calculate_tails)
            update_plan_tails(flow);
        return 1;
    }

    void choose_rack(workflow_state& flow) {
        if (flow.plan_tick > current_tick - 7 && flow.planned_finished_jobs == flow.finished_jobs &&
            flow.preferred_rack >= 0)
            return;
        i128 bytes[30]{};
        unsigned source_racks = 0;
        for (int job_index = flow.jobs_begin; job_index < flow.jobs_end; ++job_index) {
            auto& job = jobs[job_index];
            if (job.status == 2)
                continue;
            if (job.external_source_rack >= 0) {
                source_racks |= 1u << job.external_source_rack;
                bytes[job.external_source_rack] += job.external_input_bytes;
            }

            for (input_data& input_item : job.delivered_inputs) {
                bytes[input_item.source_rack] += input_item.total_bytes;
                source_racks |= 1u << input_item.source_rack;
            }
        }
        double total[30];
        for (unsigned mask = source_racks; mask; mask &= mask - 1) {
            int source = __builtin_ctz(mask);
            total[source] = (double)bytes[source];
        }
        int best_rack = -1, previous_rack = flow.preferred_rack;
        double merit = -INFINITY, best_cost = 0, best_duration = 0;
        double delay_unit = .25 * flow.value / max(5, flow.last_profitable_tick - current_tick);
        array<pair<double, int>, 30> candidates;
        int count = 0;
        int probe = flow.jobs_begin;
        while (probe < flow.jobs_end && jobs[probe].status != 0)
            ++probe;
        for (int rack_id = 0; rack_id < rack_count; ++rack_id) {
            if (probe < flow.jobs_end && (rack_max_cores[rack_id] < jobs[probe].required_cores ||
                                          rack_max_memory[rack_id] < jobs[probe].required_memory))
                continue;
            double key = cpu_prices[racks[rack_id].datacenter_id] * flow.estimated_work *
                         tenants[flow.tenant_id].duration_factor;
            if (representative_machine[rack_id] < 0 ||
                !network.node_online[network.rack_nodes[rack_id]])
                continue;
            double transfer = 0;
            for (unsigned mask = source_racks & ~(1u << rack_id); mask; mask &= mask - 1) {
                int source = __builtin_ctz(mask);
                i64 distance = network.distance(source, rack_id);
                if (distance < 0) {
                    transfer = -1;
                    break;
                }
                transfer += total[source] * distance * network_unit_fee;
            }
            if (transfer < 0)
                key = 1e50;
            else
                key += transfer;
            if (rack_id == flow.preferred_rack)
                key *= .92;
            if (probe < flow.jobs_end) {
                auto& job = jobs[probe];
                auto& hint = image_hints[rack_id * image_count + job.image_id];
                if (hint.time_bucket != current_tick / 7) {
                    hint.time_bucket = current_tick / 7;
                    hint.machine_id = -1;
                    double best_pull_score = INFINITY,
                           unit = (double)job.required_cores *
                                  cpu_prices[racks[rack_id].datacenter_id];
                    auto& rack_cache = racks[rack_id].cache;
                    for (int machine_id : machines_by_rack[rack_id]) {
                        auto& machine = machines[machine_id];
                        if (!machine.online || machine.free_cores < job.required_cores ||
                            machine.free_memory < job.required_memory)
                            continue;
                        pull_amounts amounts{};
                        for (int layer_id : images[job.image_id].sample_layers)
                            if (!machine.cache.contains(layer_id))
                                (rack_cache.contains(layer_id) ? amounts.rack_bytes
                                                               : amounts.registry_bytes) +=
                                    layer_cache::layer_sizes[layer_id];
                        double score = (double)amounts.registry_bytes * registry_unit_fee +
                                       image_prepare_time(amounts) * unit;
                        if (score < best_pull_score) {
                            best_pull_score = score;
                            hint.machine_id = machine_id;
                            if (!score)
                                break;
                        }
                    }
                    hint.pull_estimate = preview_job_image(
                        hint.machine_id < 0 ? representative_machine[rack_id] : hint.machine_id,
                        job);
                }
                key += (double)hint.pull_estimate.registry_bytes * registry_unit_fee +
                       (double)image_prepare_time(hint.pull_estimate) * job.required_cores *
                           cpu_prices[racks[rack_id].datacenter_id];
            }
            if (!flow.has_started)
                key += min(.1 * key,
                           delay_unit * rack_workload[rack_id] * inverse_rack_cores[rack_id]);
            candidates[count++] = {key, rack_id};
        }
        partial_sort(candidates.begin(), candidates.begin() + min(4, count),
                     candidates.begin() + count);
        for (int candidate_index = 0; candidate_index < min(4, count); ++candidate_index) {
            int rack_id = candidates[candidate_index].second;
            int machine = -1;
            if (!flow.has_started && probe < flow.jobs_end) {
                auto& job = jobs[probe];
                int machine_id = image_hints[rack_id * image_count + job.image_id].machine_id;
                if (machine_id >= 0 && machines[machine_id].online &&
                    machines[machine_id].free_cores >= job.required_cores &&
                    machines[machine_id].free_memory >= job.required_memory)
                    machine = machine_id;
            }
            if (!plan_on_rack(flow, rack_id, 0, machine))
                continue;
            double key = estimate_reward(flow, flow.plan_duration) - flow.plan_cost;
            if (rack_id == flow.preferred_rack)
                key += .015 * flow.value;
            if (key > merit) {
                merit = key;
                best_rack = rack_id;
                best_cost = flow.plan_cost;
                best_duration = flow.plan_duration;
                plan_buffer ^= 1;
            }
        }
        if (flow.has_started && best_rack != previous_rack) {
            double own = estimate_remaining_work(flow);
            if (previous_rack >= 0)
                rack_workload[previous_rack] -= own;
            if (best_rack >= 0)
                rack_workload[best_rack] += own;
        }
        flow.preferred_rack = best_rack;
        flow.plan_tick = current_tick;
        flow.planned_finished_jobs = flow.finished_jobs;
        if (best_rack >= 0) {
            flow.plan_cost = best_cost;
            flow.plan_duration = best_duration;
            plan_buffer ^= 1;
            update_plan_tails(flow);
            flow.detailed_plan_tick = current_tick;
            flow.detailed_plan_environment = environment_version;
        } else {
            flow.plan_duration = 1e9;
            flow.plan_cost = 1e30;
        }
    }

    void touch_workflow(workflow_state& flow, int workflow_index) {
        if (flow.schedule_tick == current_tick)
            return;
        flow.schedule_tick = current_tick;
        flow.frontier_tail = flow.estimated_work_done = 0;
        flow.priority = 0;
        active_workflows.push_back(workflow_index);
    }

    pull_amounts preview_job_image(int machine_id, job_state& job) {
        auto& machine = machines[machine_id];
        return preview_image_pull(machine.cache, racks[machine.rack_id].cache,
                                  images[job.image_id].layers);
    }

    bool has_shared_image_layer(int job_index) {
        auto& job = jobs[job_index];
        auto& image = images[job.image_id];
        // 仅给另一条可调度工作流也能复用的冷镜像留一次入场机会。
        for (int candidate_index = 0; candidate_index < min(64, (int)job_candidates.size());
             ++candidate_index) {
            auto& other_job = jobs[job_candidates[candidate_index].second];
            if (other_job.workflow_index == job.workflow_index || other_job.status != 0 ||
                remaining_quota(workflows[other_job.workflow_index].tenant_id) <
                    other_job.required_cores)
                continue;
            auto& other_image = images[other_job.image_id];
            if (!other_image.total_bytes)
                continue;
            for (int layer_id : image.sample_layers)
                for (int other_layer_id : other_image.sample_layers)
                    if (layer_id == other_layer_id &&
                        layer_cache::layer_sizes[layer_id] >=
                            max<i64>(1, min(image.total_bytes, other_image.total_bytes) / 4))
                        return 1;
        }
        return 0;
    }

    double estimate_machine_wait(job_state& job, int rack_id) {
        double best = 1e9;
        for (int machine_id : machines_by_rack[rack_id]) {
            auto& machine = machines[machine_id];
            if (!machine.online || !fits_machine_capacity(machine, job))
                continue;
            if (machine.free_cores >= job.required_cores &&
                machine.free_memory >= job.required_memory)
                return 0;
            vector<pair<double, int>> releases;
            releases.reserve(machine.running_jobs.size());
            for (int running_job_index : machine.running_jobs)
                releases.emplace_back(estimate_duration(jobs[running_job_index]),
                                      running_job_index);
            sort(releases.begin(), releases.end());
            i64 free_cores = machine.free_cores, free_memory = machine.free_memory;
            for (auto [when, running_job_index] : releases) {
                free_cores += jobs[running_job_index].required_cores;
                free_memory += jobs[running_job_index].required_memory;
                if (free_cores >= job.required_cores && free_memory >= job.required_memory) {
                    best = min(best, when);
                    break;
                }
            }
        }
        return best;
    }

    bool try_reroute(int job_index) {
        auto& job = jobs[job_index];
        auto& flow = workflows[job.workflow_index];
        if (!flow.has_started || flow.preferred_rack < 0 || flow.reroute_tick == current_tick ||
            reroute_attempts >= 8)
            return 0;
        flow.reroute_tick = current_tick;
        auto room = [&](int rack_id) {
            if (!network.node_online[network.rack_nodes[rack_id]])
                return 0;
            for (int machine_id : machines_by_rack[rack_id])
                if (machines[machine_id].online &&
                    machines[machine_id].free_cores >= job.required_cores &&
                    machines[machine_id].free_memory >= job.required_memory)
                    return 1;
            return 0;
        };
        if (room(flow.preferred_rack))
            return 0;
        ++reroute_attempts;
        workflow_state saved = flow;
        plan_snapshot prior{};
        swap_plan_snapshot(flow, prior);
        int previous_rack = flow.preferred_rack, best_rack = -1;
        double merit =
            max(0., estimate_reward(flow, flow.plan_duration +
                                              .5 * estimate_machine_wait(job, previous_rack)) -
                        flow.plan_cost) +
            .005 * flow.value;
        for (int rack_id = 0; rack_id < rack_count; ++rack_id)
            if (rack_id != previous_rack && room(rack_id) && plan_on_rack(flow, rack_id)) {
                double key = estimate_reward(flow, flow.plan_duration) - flow.plan_cost;
                if (key > merit) {
                    merit = key;
                    best_rack = rack_id;
                }
            }
        double workload = estimate_remaining_work(flow);
        if (best_rack >= 0) {
            rack_workload[previous_rack] -= workload;
            rack_workload[best_rack] += workload;
            flow.preferred_rack = best_rack;
            flow.detailed_plan_tick = flow.detailed_plan_environment = -1;
            if (place_job(job_index)) {
                flow.plan_tick = current_tick;
                flow.planned_finished_jobs = flow.finished_jobs;
                return 1;
            }
            rack_workload[previous_rack] += workload;
            rack_workload[best_rack] -= workload;
        }
        // 试算可能覆盖作业预测；未真正启动时完整恢复原锚定规划。
        flow = saved;
        swap_plan_snapshot(flow, prior);
        return 0;
    }

    bool place_job(int job_index) {
        auto& job = jobs[job_index];
        auto& flow = workflows[job.workflow_index];
        if (flow.preferred_rack >= 0 && ((flow.detailed_plan_tick == current_tick &&
                                          flow.detailed_plan_environment == environment_version) ||
                                         plan_on_rack(flow, flow.preferred_rack))) {
            flow.detailed_plan_tick = current_tick;
            flow.detailed_plan_environment = environment_version;
            flow.remaining_time = flow.plan_duration;
            flow.remaining_cost = flow.plan_cost;
        }
        double duration_factor = tenants[flow.tenant_id].duration_factor,
               duration = max(1., job.estimated_duration * duration_factor);
        double tail = estimate_tail(job, duration_factor),
               later = max(0., flow.remaining_cost - job.planned_cost);
        auto& image = images[job.image_id];
        transfer_estimate route[30];
        unsigned possible = 0;
        int end = flow.preferred_rack < 0 ? rack_count : flow.preferred_rack + 1;
        for (int rack_id = max(0, flow.preferred_rack); rack_id < end; ++rack_id) {
            if (rack_free_cores[rack_id] < job.required_cores ||
                rack_free_memory[rack_id] < job.required_memory)
                continue;
            i64 delay = 0;
            i128 fee = 0;
            for (auto& input_item : job.delivered_inputs)
                if (input_item.source_rack != rack_id) {
                    i64 distance = network.distance(input_item.source_rack, rack_id);
                    if (distance < 0) {
                        delay = -1;
                        break;
                    }
                    delay = max(delay, distance + input_item.longest_send_time);
                    fee += (i128)input_item.total_bytes * distance * network_unit_fee;
                }
            if (delay >= 0 && job.external_source_rack >= 0 &&
                job.external_source_rack != rack_id) {
                i64 distance = network.distance(job.external_source_rack, rack_id);
                if (distance < 0)
                    delay = -1;
                else {
                    delay = max(delay,
                                distance + ceil_div(job.external_input_bytes, network_bandwidth));
                    fee += (i128)job.external_input_bytes * distance * network_unit_fee;
                }
            }
            if (delay < 0)
                continue;
            possible |= 1u << rack_id;
            route[rack_id] = {delay, fee};
        }
        if (!possible)
            return 0;
        double horizon = flow.last_profitable_tick - current_tick,
               loss = (double)flow.value / max(5, flow.grace_period);
        double delay_price =
            .09 * flow.value / max(4., horizon) +
            .48 * loss *
                clamp(1. + (current_tick + flow.remaining_time - flow.deadline) / 6., 0., 1.);
        auto risk = [&](machine_state& machine) {
            return max(0, 7 - (current_tick - machine.last_alert_tick)) * (duration + 3.) *
                       job.required_cores * (.22 + .07 * min_cpu_price) +
                   max(0, 5 - (current_tick - machine.last_failure_tick)) * .15 *
                       job.required_cores;
        };
        int best_machine = -1, shared_layer = -1;
        i64 best_prepare_time = 0;
        i128 best_fee = 0;
        double best_score = INFINITY;
        bool best_investment = 0;
        vector<int>& choices =
            flow.preferred_rack >= 0 ? machines_by_rack[flow.preferred_rack] : online_machines;
        for (int id : choices) {
            auto& machine = machines[id];
            int rack_id = machine.rack_id;
            if (!machine.online || !(possible >> rack_id & 1) ||
                machine.free_cores < job.required_cores ||
                machine.free_memory < job.required_memory)
                continue;
            pull_amounts amounts = preview_job_image(id, job);
            i64 prepare_time = max(route[rack_id].transfer_time, image_prepare_time(amounts));
            if (prepare_time >= horizon)
                continue;
            i128 fee =
                route[rack_id].transfer_fee + (i128)amounts.registry_bytes * registry_unit_fee;
            double run_cost = (double)job.required_cores *
                              cpu_prices[racks[rack_id].datacenter_id] * (prepare_time + duration),
                   cost = (double)fee + run_cost;
            double key = cost + delay_price * prepare_time + risk(machine) +
                         .15 * (machine.free_cores > job.required_cores) +
                         .07 * (machine.free_memory > job.required_memory);
            if (key >= best_score)
                continue;
            double span = max(flow.remaining_time, prepare_time + duration + tail);
            double optimistic_duration =
                max(.77 * flow.remaining_time, prepare_time + max(1., .72 * duration) + .77 * tail);
            double income = .72 * reward_at(flow, current_tick + span) +
                            .28 * reward_at(flow, current_tick + optimistic_duration);
            bool investment = 0;
            if ((cost > income || cost + later > income * 1.03) && !flow.has_started &&
                !used_cache_investment && amounts.registry_bytes) {
                if (shared_layer < 0)
                    shared_layer = has_shared_image_layer(job_index);
                if (shared_layer) {
                    double allowance = .45 * estimate_reward(flow, flow.frontier_tail);
                    investment = allowance > income;
                    income = max(income, allowance);
                }
            }
            if (cost > income || cost + later > income * (flow.has_started ? 1.12 : 1.03))
                continue;
            key += (flow.value - income) * .76;
            if (key < best_score) {
                best_score = key;
                best_machine = id;
                best_prepare_time = prepare_time;
                best_fee = fee;
                best_investment = investment;
            }
        }
        if (best_machine < 0)
            return 0;
        auto& machine = machines[best_machine];
        pull_image_layers(machine.cache, racks[machine.rack_id].cache, image.layers);
        ++environment_version;
        used_cache_investment |= best_investment;
        if (!flow.has_started && flow.preferred_rack >= 0)
            rack_workload[flow.preferred_rack] += estimate_remaining_work(flow);
        pop_ready(job_index);
        job.list_position = machine.running_jobs.size();
        machine.running_jobs.push_back(job_index);
        job.status = 1;
        job.machine_id = best_machine;
        job.start_tick = current_tick;
        job.prepare_time = best_prepare_time;
        update_running_predecessors(job_index);
        machine.free_cores -= job.required_cores;
        machine.free_memory -= job.required_memory;
        flow.has_started = 1;
        update_account(job, best_machine, 1, best_fee);
        emit_command("RUN ", job.job_id, best_machine);
        return 1;
    }

    void schedule() {
        online_machines.clear();
        active_workflows.clear();
        job_candidates.clear();
        rack_free_cores.fill(0);
        rack_free_memory.fill(0);
        commands_sent = 0;
        used_cache_investment = 0;
        i64 online_cores = 0, free_cores = 0;
        reroute_attempts = 0;
        tenant_workload.assign(tenant_count, 0);
        rack_workload.fill(0);
        rack_online_cores.fill(0);
        rack_max_cores.fill(0);
        rack_max_memory.fill(0);
        representative_machine.fill(-1);
        double quality[30];
        double global = (duration_ratio_sum + 9.6) / (duration_sample_count + 8.);
        for (int tenant_id = 0; tenant_id < tenant_count; ++tenant_id) {
            auto& tenant = tenants[tenant_id];
            tenant.duration_factor =
                (tenant.duration_ratio_sum + 4 * global) / (tenant.sample_count + 4.);
            quota_limits[tenant_id] = LLONG_MAX;
            for (int ancestor_id : tenant.ancestor_path)
                quota_limits[tenant_id] = min(quota_limits[tenant_id], tenants[ancestor_id].quota);
        }
        for (workflow_state& flow : workflows)
            if (flow.has_started && flow.estimated_work > 0 &&
                current_tick < flow.last_profitable_tick) {
                double work = estimate_remaining_work(flow);
                for (int ancestor_id : tenants[flow.tenant_id].ancestor_path)
                    tenant_workload[ancestor_id] += work;
                if (flow.preferred_rack >= 0)
                    rack_workload[flow.preferred_rack] += work;
            }
        for (int id = 0; id < machine_count; ++id) {
            auto& machine = machines[id];
            if (!machine.online)
                continue;
            int rack = machine.rack_id;
            rack_online_cores[rack] += machine.total_cores;
            rack_max_cores[rack] = max(rack_max_cores[rack], machine.total_cores);
            rack_max_memory[rack] = max(rack_max_memory[rack], machine.total_memory);
            double score = machine.cache.capacity + 3 * machine.total_cores + machine.total_memory;
            if (representative_machine[rack] < 0 || score > quality[rack]) {
                quality[rack] = score;
                representative_machine[rack] = id;
            }
            for (size_t index = 0; index < machine.running_jobs.size();) {
                int job_index = machine.running_jobs[index];
                auto& job = jobs[job_index];
                auto& flow = workflows[job.workflow_index];
                if (current_tick >= flow.last_profitable_tick && commands_sent < command_limit) {
                    emit_command("KILL ", job.job_id);
                    release_job(job_index, 0);
                    ++environment_version;
                    continue;
                }
                ++index;
                online_cores += job.required_cores;
                double duration_factor = tenants[flow.tenant_id].duration_factor,
                       duration = max(1., job.estimated_duration * duration_factor);
                double left = estimate_duration(job);
                touch_workflow(flow, job.workflow_index);
                flow.frontier_tail =
                    max(flow.frontier_tail, left + estimate_tail(job, duration_factor));
                flow.estimated_work_done += job.required_cores * max(0., duration + 1 - left);
            }
            online_cores += machine.free_cores;
            free_cores += machine.free_cores;
            online_machines.push_back(id);
            rack_free_cores[machine.rack_id] =
                max(rack_free_cores[machine.rack_id], machine.free_cores);
            rack_free_memory[machine.rack_id] =
                max(rack_free_memory[machine.rack_id], machine.free_memory);
        }
        if (current_tick == total_ticks || online_machines.empty() ||
            commands_sent == command_limit)
            return;
        for (int rack_id = 0; rack_id < rack_count; ++rack_id)
            inverse_rack_cores[rack_id] = 1. / max<i64>(1, rack_online_cores[rack_id]);
        for (int tenant_id = 0; tenant_id < tenant_count; ++tenant_id)
            quota_headroom[tenant_id] = remaining_quota(tenant_id);
        vector<i64> quickest(image_count, -1); // 本轮各镜像在全部在线机器上的最小拉取时间，按需计算
        // 回收已经没有收益的就绪项，并从就绪与运行前沿估计剩余关键路径。
        for (size_t index = 0; index < ready_jobs.size();) {
            int job_index = ready_jobs[index];
            auto& job = jobs[job_index];
            auto& flow = workflows[job.workflow_index];
            if (current_tick >= flow.last_profitable_tick) {
                pop_ready(job_index);
                vector<input_data>().swap(job.delivered_inputs);
                continue;
            }
            ++index;
            if (!flow.has_started && job.retry_environment == environment_version)
                continue;
            if (job.required_cores > min(free_cores, quota_headroom[flow.tenant_id]) ||
                flow.max_unfinished_cores > quota_limits[flow.tenant_id])
                continue;
            // 单作业仅准备镜像就无法获益时，不再进入流程规划和候选队列。
            if (!flow.has_started && flow.jobs_end - flow.jobs_begin == 1) {
                i64& prepare_time = quickest[job.image_id];
                if (prepare_time < 0) {
                    prepare_time = LLONG_MAX;
                    for (int machine : online_machines) {
                        pull_amounts ancestor_id = preview_job_image(machine, job);
                        prepare_time = min(prepare_time, image_prepare_time(ancestor_id));
                    }
                }
                if (prepare_time >= flow.last_profitable_tick - current_tick) {
                    job.retry_environment = environment_version;
                    continue;
                }
            }
            touch_workflow(flow, job.workflow_index);
            flow.frontier_tail = max(flow.frontier_tail,
                                     job.critical_tail * tenants[flow.tenant_id].duration_factor +
                                         job.rough_tail - job.critical_tail);
        }
        if (active_workflows.empty())
            return;
        for (int workflow_index : active_workflows) {
            auto& flow = workflows[workflow_index];
            double duration_factor = tenants[flow.tenant_id].duration_factor;
            if (flow.max_unfinished_cores < 0) {
                flow.max_unfinished_cores = 0;
                for (int job_index = flow.jobs_begin; job_index < flow.jobs_end; ++job_index)
                    if (jobs[job_index].status != 2)
                        flow.max_unfinished_cores =
                            max(flow.max_unfinished_cores, jobs[job_index].required_cores);
            }
            // 配额未来可以恢复，因此只暂停整条工作流的新增投入，不永久放弃。
            if (flow.max_unfinished_cores > quota_limits[flow.tenant_id] ||
                flow.jobs_end - flow.jobs_begin > 40)
                continue;
            double work = max(0., flow.estimated_work * duration_factor + flow.unfinished_cores -
                                      flow.estimated_work_done);
            choose_rack(flow);
            flow.remaining_time = max(1., flow.plan_duration - (current_tick - flow.plan_tick));
            flow.remaining_cost = flow.plan_cost;
            if (!flow.has_started) {
                double ahead = 0;
                for (int ancestor_id : tenants[flow.tenant_id].ancestor_path)
                    ahead =
                        max(ahead, tenant_workload[ancestor_id] /
                                       max<i64>(1, min(tenants[ancestor_id].quota, online_cores)));
                flow.remaining_time += .45 * ahead *
                                       max(0., 1. - quota_headroom[flow.tenant_id] *
                                                        flow.remaining_time / max(1., work));
            }
            double profit = estimate_reward(flow, flow.remaining_time) - flow.remaining_cost;
            if (!flow.has_started)
                profit = max(profit, .15 * (estimate_reward(flow, flow.frontier_tail) -
                                            max(0., flow.estimated_work * duration_factor -
                                                        flow.estimated_work_done) *
                                                min_cpu_price));
            if (profit <= 0)
                continue;
            if (!flow.has_started && flow.jobs_end - flow.jobs_begin > 12 &&
                (flow.remaining_cost > .7 * flow.value ||
                 current_tick + 1.25 * flow.remaining_time > min(total_ticks, flow.deadline)))
                continue;
            double slack = max(0., flow.deadline - current_tick - flow.remaining_time),
                   urgent = 1. + 2. / (3. + slack);
            double progress = flow.has_started ? 1.8 + 1.2 * flow.finished_jobs /
                                                           max(1, flow.jobs_end - flow.jobs_begin)
                                               : 1.;
            flow.priority =
                profit * urgent * progress / (18. + .8 * work + 3. * flow.remaining_time);
        }
        for (int job_index : ready_jobs) {
            auto& job = jobs[job_index];
            auto& flow = workflows[job.workflow_index];
            if (flow.schedule_tick != current_tick || flow.priority <= 0 ||
                (!flow.has_started && job.retry_environment == environment_version) ||
                job.required_cores > min(free_cores, quota_headroom[flow.tenant_id]))
                continue;
            double lead = (job.critical_tail * tenants[flow.tenant_id].duration_factor +
                           job.rough_tail - job.critical_tail) /
                          max(1., flow.frontier_tail);
            job_candidates.push_back({-flow.priority * (.78 + .22 * lead), job_index});
        }
        int misses = max(144, machine_count), chunk = max(400, 4 * machine_count),
            candidate_count = job_candidates.size();
        for (int batch_begin = 0;
             batch_begin < candidate_count && misses > 0 && commands_sent < command_limit;
             batch_begin += chunk) {
            int batch_end = min(candidate_count, batch_begin + chunk);
            if (batch_end < candidate_count)
                nth_element(job_candidates.begin() + batch_begin,
                            job_candidates.begin() + batch_end, job_candidates.end());
            sort(job_candidates.begin() + batch_begin, job_candidates.begin() + batch_end);
            for (int index = batch_begin;
                 index < batch_end && misses > 0 && commands_sent < command_limit; ++index) {
                int job_index = job_candidates[index].second;
                auto& job = jobs[job_index];
                int owner = workflows[job.workflow_index].tenant_id;
                if (job.required_cores > free_cores || remaining_quota(owner) < job.required_cores)
                    continue;
                if (place_job(job_index) || try_reroute(job_index))
                    free_cores -= job.required_cores;
                else {
                    --misses;
                    if (!workflows[job.workflow_index].has_started)
                        jobs[job_index].retry_environment = environment_version;
                }
            }
            if (batch_end < candidate_count && misses > 0 && commands_sent < command_limit) {
                for (int tenant_id = 0; tenant_id < tenant_count; ++tenant_id)
                    quota_headroom[tenant_id] = remaining_quota(tenant_id);
                job_candidates.erase(
                    remove_if(
                        job_candidates.begin() + batch_end, job_candidates.end(),
                        [&](candidate_entry& candidate) {
                            auto& job = jobs[candidate.second];
                            return job.required_cores >
                                   min(free_cores,
                                       quota_headroom[workflows[job.workflow_index].tenant_id]);
                        }),
                    job_candidates.end());
                candidate_count = job_candidates.size();
            }
        }
    }

    void run() {
        initialize();
        for (int tick = 1; tick <= total_ticks; ++tick) {
            io.read_word(operation);
            current_tick = io.read_int();
            int event_count = io.read_int(), query_count = io.read_int();
            logs.advance_window(current_tick);
            while (event_count--)
                handle_event();
            if (compatibility_cache_dirty) {
                for (auto& job : jobs)
                    job.compatible_racks = 0;
                compatibility_cache_dirty = 0;
            }
            answer_queries(query_count);
            schedule();
            io.flush();
        }
    }
};

int main() {
#ifdef _WIN32
    _setmode(0, 32768); // Windows 文本协议输入改用二进制模式，避免逐字节转换
#endif

    scheduler solver{};
    solver.run();
}
