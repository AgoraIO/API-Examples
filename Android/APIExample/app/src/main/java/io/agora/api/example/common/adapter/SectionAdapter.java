package io.agora.api.example.common.adapter;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.List;

import io.agora.api.example.MainFragment;
import io.agora.api.example.R;
import io.agora.api.example.annotation.Example;

/**
 * Adapter for multiple sections with headers and items
 */
public class SectionAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private static final int TYPE_HEADER = 0;
    private static final int TYPE_ITEM = 1;

    private final List<Object> items = new ArrayList<>();
    private final MainFragment.OnListFragmentInteractionListener listener;

    public SectionAdapter(MainFragment.OnListFragmentInteractionListener listener) {
        this.listener = listener;
    }

    public void addSection(String title, List<Example> sectionItems) {
        items.add(title);
        if (sectionItems != null) {
            items.addAll(sectionItems);
        }
    }

    @Override
    public int getItemViewType(int position) {
        return items.get(position) instanceof String ? TYPE_HEADER : TYPE_ITEM;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        if (viewType == TYPE_HEADER) {
            View view = LayoutInflater.from(parent.getContext())
                    .inflate(R.layout.layout_main_list_section, parent, false);
            return new HeaderViewHolder(view);
        } else {
            View view = LayoutInflater.from(parent.getContext())
                    .inflate(R.layout.layout_main_list_item, parent, false);
            return new ItemViewHolder(view);
        }
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        if (holder instanceof HeaderViewHolder) {
            ((HeaderViewHolder) holder).bind((String) items.get(position));
        } else if (holder instanceof ItemViewHolder) {
            ((ItemViewHolder) holder).bind((Example) items.get(position), listener);
        }
    }

    @Override
    public int getItemCount() {
        return items.size();
    }

    static class HeaderViewHolder extends RecyclerView.ViewHolder {
        final TextView nameView;

        HeaderViewHolder(View view) {
            super(view);
            nameView = view.findViewById(R.id.item_name);
        }

        void bind(String title) {
            nameView.setText(title);
        }
    }

    static class ItemViewHolder extends RecyclerView.ViewHolder {
        final View view;
        final TextView nameView;

        ItemViewHolder(View itemView) {
            super(itemView);
            view = itemView;
            nameView = itemView.findViewById(R.id.item_name);
        }

        void bind(Example item, MainFragment.OnListFragmentInteractionListener listener) {
            nameView.setText(view.getContext().getString(item.name()));
            view.setOnClickListener(v -> {
                if (listener != null) {
                    listener.onListFragmentInteraction(item);
                }
            });
        }
    }
}
