require 'cosmos'
require 'cosmos/script'
require 'timeout'

include Cosmos::Script

# COSMOS 4 API: get_tlm_cnt counts received packets, not telemetry values.
# Read all values from one packet and reject a snapshot changed during the RPCs.
def fresh_packet(target, packet, previous_count, deadline, expected = nil)
  loop do
    if Process.clock_gettime(Process::CLOCK_MONOTONIC) >= deadline
      raise "Timed out waiting for #{expected || "fresh #{target} #{packet}"}"
    end

    before = get_tlm_cnt(target, packet)
    raise "Invalid receive count for #{target}" unless before.is_a?(Integer) && before >= 0
    raise "Telemetry receive counter reset for #{target}" if before < previous_count
    if before > previous_count
      fields = Hash[get_tlm_packet(target, packet, :RAW).map { |name, value, _limits| [name, value] }]
      after = get_tlm_cnt(target, packet)
      return [before, fields] if before == after
      raise "Telemetry receive counter reset for #{target}" if after < before
    end
    sleep 0.25
  end
end

def packet_deadline
  Process.clock_gettime(Process::CLOCK_MONOTONIC) + 30.0
end

def sample_counts(fields)
  %w[CMD_COUNT CMD_ERR_COUNT DEVICE_ERR_COUNT].map do |name|
    value = fields.fetch(name)
    raise "Invalid SAMPLE #{name}: #{value.inspect}" unless value.is_a?(Integer) && (0..255).include?(value)
    value
  end
end

def truth_time(fields)
  names = %w[YEAR DOY UTC_HOUR UTC_MINUTE UTC_SECOND]
  values = names.map { |name| fields.fetch(name) }
  valid = values.take(4).all? { |value| value.is_a?(Integer) } &&
          values[0] >= 1970 && (1..366).include?(values[1]) &&
          (0..23).include?(values[2]) && (0..59).include?(values[3]) &&
          values[4].is_a?(Numeric) && values[4].finite? && (0.0...61.0).include?(values[4])
  raise "Invalid 42 truth time: #{values.inspect}" unless valid
  values
end

# The caller also bounds the guest command. This outer deadline prevents a stuck
# COSMOS RPC from making a direct invocation wait indefinitely.
Timeout.timeout(180) do
  puts 'Begin NOS3 command and telemetry test'
  targets = get_target_list
  {'SAMPLE' => 'DEBUG', 'SAMPLE_RADIO' => 'RADIO'}.each do |target, interface|
    raise "Required target #{target} is not configured" unless targets.include?(target)
    unless get_interface_targets(interface).include?(target)
      raise "#{target} must use #{interface}; a debug-only pass does not validate CryptoLib"
    end
  end

  # Packet arrivals alone do not prove 42 advances: a simulator can retransmit
  # stale provider data. Compare the complete simulated UTC tuple, not seconds
  # alone (which wrap at a minute boundary), and do not depend on buffered files.
  truth_target = 'SIM_42_TRUTH'
  truth_packet = 'SIM_42_TRUTH_DATA'
  truth_count = get_tlm_cnt(truth_target, truth_packet)
  truth_count, fields = fresh_packet(truth_target, truth_packet, truth_count, packet_deadline)
  initial_time = truth_time(fields)
  deadline = packet_deadline
  loop do
    truth_count, fields = fresh_packet(truth_target, truth_packet, truth_count, deadline, 'advancing 42 simulated time')
    current_time = truth_time(fields)
    comparison = current_time <=> initial_time
    raise '42 simulated time went backwards' if comparison < 0
    break if comparison > 0
  end
  puts 'PASS: Fresh 42 truth telemetry advances simulated time'

  # Test both paths. SAMPLE uses debug UDP, whereas SAMPLE_RADIO traverses
  # CryptoLib and the radio. Both are declared by the stock COSMOS configuration.
  %w[SAMPLE SAMPLE_RADIO].each do |target|
    packet = 'SAMPLE_HK_TLM'
    received = get_tlm_cnt(target, packet)
    received, fields = fresh_packet(target, packet, received, packet_deadline)
    initial = sample_counts(fields)
    expected = (initial[0] + 1) % 256
    puts "#{target}: initial command=#{initial[0]} error=#{initial[1]} device_error=#{initial[2]}"
    cmd("#{target} SAMPLE_NOOP_CC")

    deadline = packet_deadline
    loop do
      received, fields = fresh_packet(target, packet, received, deadline, "#{target} CMD_COUNT #{expected} in fresh telemetry")
      current = sample_counts(fields)
      raise "#{target}: error counters changed" unless current[1..2] == initial[1..2]
      unless [initial[0], expected].include?(current[0])
        raise "#{target}: unexpected command count #{current[0]}, expected #{expected}"
      end
      break if current[0] == expected
    end

    # Verify a subsequent packet, rather than allowing unchanged cached error
    # fields to satisfy a check immediately after transmitting the command.
    received, fields = fresh_packet(target, packet, received, packet_deadline)
    final = sample_counts(fields)
    raise "#{target}: command result did not remain stable" unless final == [expected, initial[1], initial[2]]
    puts "PASS: #{target} fresh telemetry confirms CMD_COUNT #{initial[0]} -> #{final[0]} with unchanged errors"
  end
  puts 'NOS3 COMMAND AND TELEMETRY TEST COMPLETED SUCCESSFULLY'
end
