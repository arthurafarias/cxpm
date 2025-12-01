#include <gst/gst.h>

int main(int argc, char *argv[]) {
  GstElement *pipeline, *source, *sink;
  GstBus *bus;
  GstMessage *msg;

  gst_init(&argc, &argv);

  /* Create elements */
  source = gst_element_factory_make("audiotestsrc", "source");
  sink = gst_element_factory_make("autoaudiosink", "audio_output");

  /* Create the empty pipeline */
  pipeline = gst_pipeline_new("audio-test-pipeline");

  if (!pipeline || !source || !sink) {
    g_printerr("Failed to create elements\n");
    return -1;
  }

  /* Configure audiotestsrc if desired */
  // g_object_set(source, "freq", 440.0, NULL);

  /* Build the pipeline */
  gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
  if (!gst_element_link(source, sink)) {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  /* Start playing */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  /* Wait for EOS or ERROR */
  bus = gst_element_get_bus(pipeline);
  msg = gst_bus_timed_pop_filtered(
      bus, GST_CLOCK_TIME_NONE,
      (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  /* Parse message */
  if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &err, &debug_info);
      g_printerr("Error: %s\n", err->message);
      g_error_free(err);
      g_free(debug_info);
      break;
    case GST_MESSAGE_EOS:
      g_print("End of stream\n");
      break;
    default:
      break;
    }
    gst_message_unref(msg);
  }

  /* Free resources */
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}