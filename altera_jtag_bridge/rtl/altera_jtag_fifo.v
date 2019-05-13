
// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on


module altera_jtag_fifo (
  // inputs
  clk,
  rst_n,
  write,
  writedata,
  // outputs
  readdata,
  dataavailable,
  readyfordata
)
  /* synthesis ALTERA_ATTRIBUTE = "SUPPRESS_DA_RULE_INTERNAL=\"R101,C106,D101,D103\"" */ ;

  input         clk;
  input         rst_n;
  input         write;
  input   [7:0] writedata;
  output  [7:0] readdata;
  output        dataavailable;
  output        readyfordata;

  wire    [7:0] readdata;
  wire          dataavailable;
  wire          readyfordata;

  wire          fifo_clear;

  wire    [7:0] rfifo_wdata;
  wire          rfifo_empty;
  wire          rfifo_full;
  wire    [7:0] rfifo_rdata;
  wire          rfifo_rd;
  wire          rfifo_wr;
//  wire    [5:0] rfifo_used;

  reg     [7:0] wfifo_wdata;
  wire          wfifo_empty;
  wire          wfifo_full;
  wire    [7:0] wfifo_rdata;
  wire          wfifo_rd;
  reg           wfifo_wr;
//  wire    [5:0] wfifo_used;

  wire          r_ena;
  wire          t_ena;
  reg           r_val;
  reg           t_dav;
//  wire          t_pause;


  assign fifo_clear = ~rst_n;
  assign wfifo_rd = r_ena & ~wfifo_empty;
  assign rfifo_wr = t_ena & ~rfifo_full;

  assign dataavailable  = ~rfifo_empty;
  assign readyfordata   = ~wfifo_full;
  assign rfifo_rd       = 1'b1;
  assign readdata       = rfifo_rdata;


  always @(posedge clk or negedge rst_n)
    begin
      if (rst_n == 0)
        begin
          r_val <= 1'b0;
          t_dav <= 1'b1;
        end
      else 
        begin
          r_val <= r_ena & ~wfifo_empty;
          t_dav <= ~rfifo_full;
        end
    end

  always @(posedge clk or negedge rst_n)
    begin
      if (rst_n == 0)
        begin
          wfifo_wr <= 1'b0;
        end
      else 
        begin
          wfifo_wr <= 1'b0;
          // write
          if (write) begin
            wfifo_wr    <= ~wfifo_full;
            wfifo_wdata <= writedata;
          end
        end
    end


  scfifo the_wfifo (
    .aclr  (fifo_clear),
    .clock (clk),
    .data  (wfifo_wdata),
    .empty (wfifo_empty),
    .full  (wfifo_full),
    .q     (wfifo_rdata),
    .rdreq (wfifo_rd),
//    .usedw (wfifo_used),
    .wrreq (wfifo_wr)
  );
  defparam the_wfifo.lpm_hint = "RAM_BLOCK_TYPE=AUTO",
           the_wfifo.lpm_numwords = 64,
           the_wfifo.lpm_showahead = "OFF",
           the_wfifo.lpm_type = "scfifo",
           the_wfifo.lpm_width = 8,
           the_wfifo.lpm_widthu = 6,
           the_wfifo.overflow_checking = "OFF",
           the_wfifo.underflow_checking = "OFF",
           the_wfifo.use_eab = "ON";
  

  scfifo the_rfifo (
    .aclr  (fifo_clear),
    .clock (clk),
    .data  (rfifo_wdata),
    .empty (rfifo_empty),
    .full  (rfifo_full),
    .q     (rfifo_rdata),
    .rdreq (rfifo_rd),
//    .usedw (rfifo_used),
    .wrreq (rfifo_wr)
  );
  defparam the_rfifo.lpm_hint = "RAM_BLOCK_TYPE=AUTO",
           the_rfifo.lpm_numwords = 64,
           the_rfifo.lpm_showahead = "OFF",
           the_rfifo.lpm_type = "scfifo",
           the_rfifo.lpm_width = 8,
           the_rfifo.lpm_widthu = 6,
           the_rfifo.overflow_checking = "OFF",
           the_rfifo.underflow_checking = "OFF",
           the_rfifo.use_eab = "ON";
  

  alt_jtag_atlantic the_alt_jtag_atlantic (
    .clk (clk),
    .r_dat (wfifo_rdata), // (in)  input data to send out via jtag
    .r_ena (r_ena),       // (out) ready to accept input data
    .r_val (r_val),       // (in)  input data valid (should only be set high, if r_ena is high)
    .rst_n (rst_n),
    .t_dat (rfifo_wdata), // (out) output data received via jtag
    .t_dav (t_dav),       // (in)  read fifo ready to accept data? (would be 'r_ena' of rfifo)
    .t_ena (t_ena)        // (out) output data valid
//    .t_pause (t_pause)    // (out) (currently transmitting via jtag??? -> don't read or write data???)
  );
  defparam the_alt_jtag_atlantic.INSTANCE_ID = 0,
           the_alt_jtag_atlantic.LOG2_RXFIFO_DEPTH = 6,
           the_alt_jtag_atlantic.LOG2_TXFIFO_DEPTH = 6,
           the_alt_jtag_atlantic.SLD_AUTO_INSTANCE_INDEX = "YES";


endmodule
