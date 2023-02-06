# Test status

apu_test:

| Test | Result |
| --- | --- |
| 1-len\_ctr | :ok: |
| 2-len\_table | :ok: |
| 3-irq\_flag | :ok: |
| 4-jitter | :ok: |
| 5-len\_timing | :ok: |
| 6-irq\_flag\_timing | :ok: |
| 7-dmc\_basics | :ok: |
| 8-dmc\_rates | :ok: |

blargg_apu:

| Test | Result |
| --- | --- |
| 01.len\_ctr | :ok: |
| 02.len\_table | :ok: |
| 03.irq\_flag | :ok: |
| 04.clock\_jitter | :ok: |
| 05.len\_timing\_mode0 | :ok: |
| 06.len\_timing\_mode1 | :ok: |
| 07.irq\_flag\_timing | :ok: |
| 08.irq\_timing | :ok: |
| 09.reset\_timing | :ok: |
| 10.len\_halt\_timing | :x: |
| 11.len\_reload\_timing | :x: |

blargg_nes_cpu_test5:

| Test | Result |
| --- | --- |
| cpu | :ok: |
| official | :ok: |

blargg_ppu_tests:

| Test | Result |
| --- | --- |
| palette\_ram | :ok: |
| power\_up\_palette | :x: |
| sprite\_ram | :ok: |
| vbl\_clear\_time | :ok: |
| vram\_access | :ok: |

branch_timing_tests:

| Test | Result |
| --- | --- |
| 1.Branch\_Basics | :ok: |
| 2.Backward\_Branch | :ok: |
| 3.Forward\_Branch | :ok: |

cpu_dummy_reads:

| Test | Result |
| --- | --- |
| cpu\_dummy\_reads | :ok: |

cpu_dummy_writes:

| Test | Result |
| --- | --- |
| cpu\_dummy\_writes\_oam | :ok: |
| cpu\_dummy\_writes\_ppumem | :ok: |

cpu_exec_space:

| Test | Result |
| --- | --- |
| test\_cpu\_exec\_space\_apu | :ok: |
| test\_cpu\_exec\_space\_ppuio | :ok: |

cpu_interrupts_v2:

| Test | Result |
| --- | --- |
| 1-cli\_latency | :ok: |
| 2-nmi\_and\_brk | :ok: |
| 3-nmi\_and\_irq | :ok: |
| 4-irq\_and\_dma | :ok: |
| 5-branch\_delays\_irq | :ok: |

cpu_timing_test6:

| Test | Result |
| --- | --- |
| cpu\_timing\_test | :ok: |

dmc_dma_during_read4:

| Test | Result |
| --- | --- |
| dma\_2007\_read | :x: |
| dma\_2007\_write | :ok: |
| dma\_4016\_read | :ok: |
| double\_2007\_read | :x: |
| read\_write\_2007 | :ok: |

dmc_tests:

| Test | Result |
| --- | --- |
| buffer\_retained | :x: |
| latency | :x: |
| status | :x: |
| status\_irq | :x: |

instr_misc:

| Test | Result |
| --- | --- |
| 01-abs\_x\_wrap | :ok: |
| 02-branch\_wrap | :ok: |
| 03-dummy\_reads | :ok: |
| 04-dummy\_reads\_apu | :ok: |

instr_test-v5:

| Test | Result |
| --- | --- |
| 01-basics | :ok: |
| 02-implied | :ok: |
| 03-immediate | :ok: |
| 04-zero\_page | :ok: |
| 05-zp\_xy | :ok: |
| 06-absolute | :ok: |
| 07-abs\_xy | :ok: |
| 08-ind\_x | :ok: |
| 09-ind\_y | :ok: |
| 10-branches | :ok: |
| 11-stack | :ok: |
| 12-jmp\_jsr | :ok: |
| 13-rts | :ok: |
| 14-rti | :ok: |
| 15-brk | :ok: |
| 16-special | :ok: |

instr_timing:

| Test | Result |
| --- | --- |
| 1-instr\_timing | :ok: |
| 2-branch\_timing | :ok: |

mmc3_test:

| Test | Result |
| --- | --- |
| 1-clocking | :ok: |
| 2-details | :ok: |
| 3-A12\_clocking | :ok: |
| 4-scanline\_timing | :ok: |
| 5-MMC3 | :ok: |
| 6-MMC3\_alt | :x: |

nes_instr_test:

| Test | Result |
| --- | --- |
| 01-implied | :ok: |
| 02-immediate | :ok: |
| 03-zero\_page | :ok: |
| 04-zp\_xy | :ok: |
| 05-absolute | :ok: |
| 06-abs\_xy | :ok: |
| 07-ind\_x | :ok: |
| 08-ind\_y | :ok: |
| 09-branches | :ok: |
| 10-stack | :ok: |
| 11-special | :ok: |

oam_read:

| Test | Result |
| --- | --- |
| oam\_read | :ok: |

oam_stress:

| Test | Result |
| --- | --- |
| oam\_stress | :ok: |

ppu_open_bus:

| Test | Result |
| --- | --- |
| ppu\_open\_bus | :x: |

ppu_read_buffer:

| Test | Result |
| --- | --- |
| test\_ppu\_read\_buffer | :ok: |

ppu_vbl_nmi:

| Test | Result |
| --- | --- |
| 01-vbl\_basics | :ok: |
| 02-vbl\_set\_time | :ok: |
| 03-vbl\_clear\_time | :ok: |
| 04-nmi\_control | :ok: |
| 05-nmi\_timing | :ok: |
| 06-suppression | :ok: |
| 07-nmi\_on\_timing | :ok: |
| 08-nmi\_off\_timing | :ok: |
| 09-even\_odd\_frames | :ok: |
| 10-even\_odd\_timing | :ok: |

sprdma_and_dmc_dma:

| Test | Result |
| --- | --- |
| sprdma\_and\_dmc\_dma | :x: |
| sprdma\_and\_dmc\_dma\_512 | :x: |

sprite_hit_tests:

| Test | Result |
| --- | --- |
| 01.basics | :ok: |
| 02.alignment | :ok: |
| 03.corners | :ok: |
| 04.flip | :ok: |
| 05.left\_clip | :ok: |
| 06.right\_edge | :ok: |
| 07.screen\_bottom | :ok: |
| 08.double\_height | :ok: |
| 09.timing\_basics | :ok: |
| 10.timing\_order | :ok: |
| 11.edge\_timing | :ok: |

sprite_overflow_tests:

| Test | Result |
| --- | --- |
| 1.Basics | :ok: |
| 2.Details | :ok: |
| 3.Timing | :ok: |
| 4.Obscure | :ok: |
| 5.Emulator | :ok: |

vbl_nmi_timing:

| Test | Result |
| --- | --- |
| 1.frame\_basics | :ok: |
| 2.vbl\_timing | :ok: |
| 3.even\_odd\_frames | :ok: |
| 4.vbl\_clear\_timing | :ok: |
| 5.nmi\_suppression | :ok: |
| 6.nmi\_disable | :ok: |
| 7.nmi\_timing | :ok: |

