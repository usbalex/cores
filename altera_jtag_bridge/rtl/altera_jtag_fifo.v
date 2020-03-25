
// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on


module altera_jtag_fifo (
  // inputs
  i_clk,
  i_rst_n,
  i_read,
  i_write,
  i_writedata,
  // outputs
  o_readdata,
  o_dataavailable,
  o_readyfordata
)
  /* synthesis ALTERA_ATTRIBUTE = "SUPPRESS_DA_RULE_INTERNAL=\"R101,C106,D101,D103\"" */ ;

  input         i_clk;
  input         i_rst_n;
  input         i_read;
  input         i_write;
  input   [7:0] i_writedata;
  output  [7:0] o_readdata;
  output        o_dataavailable;
  output        o_readyfordata;

  wire    [7:0] o_readdata;
  wire          o_dataavailable;
  wire          o_readyfordata;

  wire          fifo_clear;

  wire    [7:0] rfifo_wdata;
  wire          rfifo_empty;
  wire          rfifo_full;
  wire    [7:0] rfifo_rdata;
  wire          rfifo_rd;
  wire          rfifo_wr;

  wire    [7:0] wfifo_wdata;
  wire          wfifo_empty;
  wire          wfifo_full;
  wire    [7:0] wfifo_rdata;
  wire          wfifo_rd;
  wire          wfifo_wr;

  wire          r_ena;
  wire          t_ena;
  reg           r_val;
  reg           t_dav;
  wire          t_pause;

  reg           wfifo_full_del;


  assign fifo_clear = ~i_rst_n;

  // jtag_atlantic <-> FIFOs
  assign wfifo_rd = r_ena & ~wfifo_empty;
  assign rfifo_wr = t_ena & ~rfifo_full;

  // interface <-> FIFOs
  assign o_dataavailable  = ~rfifo_empty;
  assign o_readdata       = rfifo_rdata;
  assign o_readyfordata   = ~wfifo_full;
  assign rfifo_rd       = i_read;
  assign wfifo_wr       = i_write & ~wfifo_full;
  assign wfifo_wdata    = i_writedata;


  always @(posedge i_clk or negedge i_rst_n)
    begin
      if (i_rst_n == 0)
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


  // module inputs -> FIFO -> jtag chain
  scfifo the_wfifo (
    .aclr  (fifo_clear),
    .clock (i_clk),
    .data  (wfifo_wdata),
    .empty (wfifo_empty),
    .full  (wfifo_full),
    .q     (wfifo_rdata),
    .rdreq (wfifo_rd),
    .wrreq (wfifo_wr)
  );
  defparam the_wfifo.lpm_hint = "RAM_BLOCK_TYPE=AUTO",
           the_wfifo.lpm_numwords = 64,
           the_wfifo.lpm_showahead = "OFF",
           the_wfifo.lpm_type = "scfifo",
           the_wfifo.lpm_width = 8,
           the_wfifo.lpm_widthu = 6,
           the_wfifo.overflow_checking = "ON",
           the_wfifo.underflow_checking = "ON",
           the_wfifo.use_eab = "ON";
  

  // jtag chain -> FIFO -> module outputs
  scfifo the_rfifo (
    .aclr  (fifo_clear),
    .clock (i_clk),
    .data  (rfifo_wdata),
    .empty (rfifo_empty),
    .full  (rfifo_full),
    .q     (rfifo_rdata),
    .rdreq (rfifo_rd),
    .wrreq (rfifo_wr)
  );
  defparam the_rfifo.lpm_hint = "RAM_BLOCK_TYPE=AUTO",
           the_rfifo.lpm_numwords = 64,
           the_rfifo.lpm_showahead = "ON",
           the_rfifo.lpm_type = "scfifo",
           the_rfifo.lpm_width = 8,
           the_rfifo.lpm_widthu = 6,
           the_rfifo.overflow_checking = "ON",
           the_rfifo.underflow_checking = "ON",
           the_rfifo.use_eab = "ON";
  

  alt_jtag_atlantic the_alt_jtag_atlantic (
    .clk (i_clk),
    .r_dat (wfifo_rdata), // (in)  input data to send out via jtag
    .r_ena (r_ena),       // (out) ready to accept input data
    .r_val (r_val),       // (in)  input data valid (should only be set high, if r_ena is high)
    .rst_n (i_rst_n),
    .t_dat (rfifo_wdata), // (out) output data received via jtag
    .t_dav (t_dav),       // (in)  read fifo ready to accept data? (would be 'r_ena' of rfifo)
    .t_ena (t_ena),       // (out) output data valid
    .t_pause (t_pause)    // (out) (currently not transmitting via jtag??? -> don't read or write data???)
  );
  defparam the_alt_jtag_atlantic.INSTANCE_ID = 0,
           the_alt_jtag_atlantic.LOG2_RXFIFO_DEPTH = 6,
           the_alt_jtag_atlantic.LOG2_TXFIFO_DEPTH = 6,
           the_alt_jtag_atlantic.SLD_AUTO_INSTANCE_INDEX = "YES";

endmodule

